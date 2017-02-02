#include "object_tree_model.hpp"
#include "../core/object.hpp"

using namespace ev::atk;

object_tree_model_base_t::object_tree_model_base_t(object_t* root, qobject* parent)
    : QAbstractItemModel(parent), m_root(root)
{
    connect(m_root, SIGNAL(children_changed()), this, SLOT(on_hierarchy_changed()));
}

object_t* object_tree_model_base_t::root() const
{
    return m_root;
}

QModelIndex object_tree_model_base_t::index(int row, int column, const QModelIndex& parent_index) const
{
    if (!hasIndex(row, column, parent_index)) return QModelIndex();

    object_t* parent_obj = nullptr;
    if (!parent_index.isValid()) {
        parent_obj = m_root;
    }
    else {
        parent_obj = objectForIndex(parent_index);
    }

    object_t* child_obj = parent_obj->children().at(row);

    if (child_obj)
        return createIndex(row, column, child_obj);
    else
        return QModelIndex();
}

QModelIndex object_tree_model_base_t::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    object_t* childObject = objectForIndex(index);
    object_t* parent_obj  = childObject->get_parent();
    if (parent_obj == m_root) {
        return QModelIndex();
    }
    return createIndex(parent_obj->index_in_parent(), 0, parent_obj);
}

int object_tree_model_base_t::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) return 0;

    object_t* parent_obj = nullptr;

    if (!parent.isValid()) {
        parent_obj = m_root;
    }
    else {
        parent_obj = objectForIndex(parent);
    }
    return parent_obj->children().size();
}

int object_tree_model_base_t::columnCount(const QModelIndex&) const
{
    // return 2;
    return 1;
}

QVariant object_tree_model_base_t::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    //    if (index.column() == 1) {
    //        switch (role) {
    //            case IsClosableRole: {
    //                object_t* object = objectForIndex(index);
    //                return object->flag(Object::Closable);
    //            }
    //            default: break;
    //        }
    //        return QVariant();
    //    }

    object_t* object = objectForIndex(index);
    switch (role) {
        case Qt::DisplayRole: {
            QString name  = object->name();
            if (name.isEmpty()) name = "unamed";
            //            if (item->isModified()) name += "  *";
            return name;
        }
        case Qt::EditRole: {
            return object->name();
        }
        default: break;
    }

    if (m_data_getter) return m_data_getter(object,role);
    return qvariant();
}

bool object_tree_model_base_t::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() != 0) {
        return false;
    }
    switch (role) {
        case Qt::EditRole: {
            object_t* object = objectForIndex(index);
            object->set_name(value.toString());
            Q_EMIT dataChanged(index, index);
            return true;
        }
        default: break;
    }
    return false;
}

QVariant object_tree_model_base_t::headerData(int section,
                                              Qt::Orientation orientation,
                                              int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return "Object";
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags object_tree_model_base_t::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags =
        QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;

    //    if (!m_readOnly && index.column() == 0) {
    //        flags |= Qt::ItemIsEditable;
    //    }

    return flags;
}

void object_tree_model_base_t::on_hierarchy_changed()
{
    beginResetModel();
    endResetModel();
    Q_EMIT expandRequest(createIndex(0, 0, m_root));
}

void object_tree_model_base_t::on_data_changed(object_t* object)
{
    QModelIndex from = createIndex(object->index_in_parent(), 0, object);
    QModelIndex to   = createIndex(object->index_in_parent(), 1, object);

    if (from.isValid()) {
        dataChanged(from, to);
    }
}

object_t* object_tree_model_base_t::objectForIndex(const QModelIndex& index) const
{
    return static_cast<object_t*>(index.internalPointer());
}
