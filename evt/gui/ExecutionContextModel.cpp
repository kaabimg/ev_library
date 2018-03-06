#include "ExecutionContextModel.hpp"

using namespace evt;

struct ExecutionContextModel::Impl {
    ev::execution_context context;
    ev::execution_cxt_data::item root;
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
    d->root.payload = ec.data_ptr();
    endResetModel();
}

void ExecutionContextModel::reset()
{
    beginResetModel();
    endResetModel();
}

Qt::ItemFlags ExecutionContextModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
int ExecutionContextModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
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

    if (!parent.isValid()) {
        return d->context.subitem_count();
    }

    const ev::execution_cxt_data::item* item =
        reinterpret_cast<const ev::execution_cxt_data::item*>(parent.internalPointer());

    int rc = 0;

    if (item->is_execution_context()) rc = item->context().subitem_count();

    return rc;
}

QModelIndex ExecutionContextModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    if (column != 0) return QModelIndex();

    if (!d->context) {
        return QModelIndex();
    }

    const ev::execution_cxt_data::item* parentItem;

    if (!parent.isValid())
        parentItem = &d->root;
    else
        parentItem = static_cast<const ev::execution_cxt_data::item*>(parent.internalPointer());

    if (!parentItem->is_execution_context()) return QModelIndex();

    const ev::execution_cxt_data::item* childItem = &parentItem->context_data().subitems.data[row];
    return createIndex(row, column, (void*)childItem);
}

QModelIndex ExecutionContextModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    const ev::execution_cxt_data::item* childItem =
        static_cast<const ev::execution_cxt_data::item*>(index.internalPointer());
    const ev::execution_cxt_data* parentItem = childItem->parent;

    if (parentItem == d->context.data_ptr()) return QModelIndex();

    auto grandParent = parentItem->parent;

    int row = 0;
    if (grandParent) {
        int count = grandParent->subitems.size;
        for (; row < count; ++row) {
            const ev::execution_cxt_data::item& item = grandParent->subitems.data[row];
            if (item.is_execution_context() && &item.context_data() == parentItem) break;
        }
    }

    return createIndex(row, 0, &grandParent->subitems.data[row]);
}

QVariant ExecutionContextModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    const ev::execution_cxt_data::item* node =
        static_cast<const ev::execution_cxt_data::item*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column() == 0) {
                if (node->is_message())
                    return QString::fromStdString(node->message().message);
                else {
                    auto cxt = node->context();
                    QString taskStatus = QString::fromStdString(cxt.name());
                    taskStatus.append(" ");
                    taskStatus.append(QString::fromStdString(ev::to_string(cxt.status())));
                    taskStatus.append(" : ");
                    taskStatus.append(QString::number(cxt.progress())).append('%');

                    return taskStatus;
                }
            }
        }
    }
    return QVariant();
}
