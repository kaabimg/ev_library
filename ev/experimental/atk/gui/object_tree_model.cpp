#include "object_tree_model.hpp"
#include "../core/object.hpp"

using namespace ev::atk;

object_model_base_t::object_model_base_t(object_t* root, qobject* parent)
    : QAbstractItemModel(parent), m_root(root)
{
    connect(m_root, SIGNAL(children_changed()), this, SLOT(on_hierarchy_changed()));
}

object_t* object_model_base_t::root() const
{
    return m_root;
}

qmodelindex object_model_base_t::index(int row,
                                            int column,
                                            const qmodelindex& parent_index) const
{
    if (!hasIndex(row, column, parent_index)) return qmodelindex();

    object_t* parent_obj = nullptr;
    if (!parent_index.isValid()) {
        parent_obj = m_root;
    }
    else {
        parent_obj = object_for_index(parent_index);
    }

    object_t* child_obj = parent_obj->children().at(row);

    if (child_obj)
        return createIndex(row, column, child_obj);
    else
        return qmodelindex();
}

qmodelindex object_model_base_t::parent(const qmodelindex& index) const
{
    if (!index.isValid()) return qmodelindex();

    object_t* childObject = object_for_index(index);
    object_t* parent_obj  = childObject->get_parent();
    if (parent_obj == m_root) {
        return qmodelindex();
    }
    return createIndex(parent_obj->index_in_parent(), 0, parent_obj);
}

int object_model_base_t::rowCount(const qmodelindex& parent) const
{
    if (parent.column() > 0) return 0;

    object_t* parent_obj = nullptr;

    if (!parent.isValid()) {
        parent_obj = m_root;
    }
    else {
        parent_obj = object_for_index(parent);
    }
    return parent_obj->children().size();
}

int object_model_base_t::columnCount(const qmodelindex&) const
{
    if (m_proxy.column_count != -1) return m_proxy.column_count;
    return 1;
}

qvariant object_model_base_t::data(const qmodelindex& index, int role) const
{
    if (!index.isValid()) return qvariant();

    object_t* object = object_for_index(index);

    if (m_proxy.get_data) return m_proxy.get_data(object, role);

    switch (role) {
        case Qt::DisplayRole: {
            QString name             = object->name();
            if (name.isEmpty()) name = "unamed";
            return name;
        }
        default: break;
    }

    return qvariant();
}

bool object_model_base_t::setData(const qmodelindex& index, const qvariant& value, int role)
{
    if (index.column() != 0) {
        return false;
    }
    object_t* object = object_for_index(index);

    if (m_proxy.set_data) return m_proxy.set_data(object, role, value);

    switch (role) {
        case Qt::EditRole: {
            object->set_name(value.toString());
            Q_EMIT dataChanged(index, index);
            return true;
        }
        default: break;
    }
    return false;
}

qvariant object_model_base_t::headerData(int section,
                                              Qt::Orientation orientation,
                                              int role) const
{
    if (m_proxy.get_header_data) return m_proxy.get_header_data(section, orientation, role);
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return "Object";
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags object_model_base_t::flags(const qmodelindex& index) const
{
    if (m_proxy.get_flags) {
        return m_proxy.get_flags(object_for_index(index));
    }

    Qt::ItemFlags flags =
        QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;

    return flags;
}

void object_model_base_t::on_hierarchy_changed()
{
    beginResetModel();
    endResetModel();
    Q_EMIT expand_request(createIndex(0, 0, m_root));
}

void object_model_base_t::on_data_changed(object_t* object)
{
    qmodelindex from = createIndex(object->index_in_parent(), 0, object);
    qmodelindex to   = createIndex(object->index_in_parent(), 1, object);

    if (from.isValid()) {
        dataChanged(from, to);
    }
}

object_t* object_model_base_t::object_for_index(const qmodelindex& index) const
{
    return static_cast<object_t*>(index.internalPointer());
}

//////////////////////////////////////////////////////////////////////////////////////////

object_model_t::object_model_t(object_t* root, qobject* parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(new object_model_base_t(root, this));
}

object_model_t::~object_model_t()
{
}

object_model_base_t* object_model_t::source() const
{
    return qobject_cast<object_model_base_t*>(sourceModel());
}

bool object_model_t::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    return m_proxy.filter(static_cast<object_t*>(index.internalPointer()));
}

bool object_model_t::lessThan(const QModelIndex& source_left,
                                   const QModelIndex& source_right) const
{
    return m_proxy.less_than(static_cast<object_t*>(source_left.internalPointer()),
                             static_cast<object_t*>(source_right.internalPointer()));
}

void object_model_t::set_data_proxy(const object_data_proxy_t &proxy)
{
    beginResetModel();
    source()->m_proxy = proxy;
    endResetModel();
}

void object_model_t::set_filter_sort_proxy(const filter_sort_proxy_t &proxy)
{
    beginResetModel();
    m_proxy = proxy;
    endResetModel();
}

void object_model_t::set_column_count(int count)
{
    beginResetModel();
    source()->m_proxy.column_count = count;
    endResetModel();
}
