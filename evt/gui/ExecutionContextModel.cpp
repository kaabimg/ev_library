#include "ExecutionContextModel.hpp"

using namespace evt;
struct ExecutionContextModel::Impl {
    ev::execution_context context;
};

ExecutionContextModel::ExecutionContextModel(QObject* parent)
{
}

ExecutionContextModel::~ExecutionContextModel()
{
}

void ExecutionContextModel::setExecutionContext(ev::execution_context ec)
{
    beginResetModel();
    d->context = ec;
    endResetModel();
}

Qt::ItemFlags ExecutionContextModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
int ExecutionContextModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

QVariant ExecutionContextModel::headerData(int /*section*/,
                                           Qt::Orientation /*orientation*/,
                                           int /*role*/) const
{
    return QVariant();
}

int ExecutionContextModel::rowCount(const QModelIndex& parent) const
{
    if (!d->context) {
        return 0;
    }
    if (parent.column() > 0) return 0;

    if (!parent.isValid()) return d->context.subitem_count();

    const ev::execution_cxt_data::subitem* item =
        reinterpret_cast<const ev::execution_cxt_data::subitem*>(parent.internalPointer());

    if (item->is_message()) return 0;
    return item->context().subitem_count();
}

QModelIndex ExecutionContextModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!d->context) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        return createIndex(row, column, reinterpret_cast<quintptr>(&d->context.subitem_at(row)));
    }

    const ev::execution_cxt_data::subitem* subitem =
        reinterpret_cast<const ev::execution_cxt_data::subitem*>(parent.internalPointer());

    if (subitem->is_exection_context()) {
        auto context = subitem->context();

        return createIndex(row, column, reinterpret_cast<quintptr>(&context.subitem_at(row)));
    }
    return QModelIndex();
}

QModelIndex ExecutionContextModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    if (!d->context) {
        return QModelIndex();
    }

    const ev::execution_context::subitem* childNode =
        reinterpret_cast<const ev::execution_context::subitem*>(index.internalPointer());

    const ev::execution_cxt_data* parent = childNode->parent;

    if (!parent || parent == d->context.data_ptr()) return QModelIndex();

    int distance = std::distance(&parent->subitems.data[0], childNode);

    return createIndex(distance, 0, reinterpret_cast<quintptr>(parent));
}

QVariant ExecutionContextModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    const ev::execution_cxt_data::subitem* node =
        static_cast<const ev::execution_cxt_data::subitem*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column() == 0) {
                if (node->is_message())
                    return QString::fromStdString(node->message().message);
                else
                    return QString::fromStdString(node->context().name());
            }
        }
    }
    return QVariant();
}
