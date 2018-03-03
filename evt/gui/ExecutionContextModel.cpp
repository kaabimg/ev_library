#include "ExecutionContextModel.hpp"

using namespace evt;
struct ExecutionContextModel::Impl {
    ev::execution_context executionContext;
    ev::execution_context_subitem root;
};

ExecutionContextModel::ExecutionContextModel(QObject* parent)
{
}

ExecutionContextModel::~ExecutionContextModel()
{
}

void ExecutionContextModel::setRoot(ev::execution_context ec)
{
    beginResetModel();
    d->executionContext = ec;
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
}

QModelIndex ExecutionContextModel::index(int row, int column, const QModelIndex& parent) const
{
//    if (!hasIndex(row, column, parent)) return QModelIndex();

//    if (!d->executionContext) {
//        return QModelIndex();
//    }

//    QModelIndex index;
//    index.

//    const ev::execution_context_subitem* parentNode;

//    if (!parent.isValid())
//        parentNode = &d->root;
//    else
//        parentNode = reinterpret_cast<const ev::execution_context_subitem*>(parent.internalPointer());

//    if (!parentNode) {
//        return QModelIndex();
//    }

//    const ev::execution_context_subitem& childNode = parentNode->subitem_at(row);
//    if (childNode)
//        return createIndex(row, column, reinterpret_cast<quintptr>(childNode));
//    else
        return QModelIndex();
}

QVariant ExecutionContextModel::data(const QModelIndex& index, int role) const
{
}
QModelIndex ExecutionContextModel::parent(const QModelIndex& index) const
{
//    if (!index.isValid()) return QModelIndex();

//    if (!d->executionContext) {
//        return QModelIndex();
//    }

//    const ev::execution_context_subitem* childNode =
//        reinterpret_cast<const ev::execution_context_subitem*>(index.internalPointer());
//    const ev::execution_context_subitem* parentNode = childNode->parent;

//    if (parentNode == _impl->parseResult.rootNode.get()) return QModelIndex();

//    int distance = std::distance(
//        parentNode->children.begin(),
//        std::find(parentNode->children.begin(), parentNode->children.end(), childNode));
//    return createIndex(distance, 0, reinterpret_cast<quintptr>(parentNode));
}
