#include "ProgramTreeModel.hpp"

#include <p4cl/parser/ProgramTree.hpp>
#include <p4cl/parser/Parser.hpp>
#include <evt/gui/Style.hpp>

#include "../UiUtils.hpp"

#include <QSize>

using namespace p4s;

struct ProgramTreeModel::Impl {
    p4cl::parser::Result parseResult;
};

ProgramTreeModel::ProgramTreeModel(QObject* parent) : QAbstractItemModel(parent)
{
}

ProgramTreeModel::~ProgramTreeModel()
{
}

void ProgramTreeModel::setRoot(p4cl::parser::Result result)
{
    beginResetModel();
    d->parseResult = std::move(result);
    endResetModel();
}

const p4cl::ast::Node* ProgramTreeModel::nodeAt(const QModelIndex& index) const
{
    if (!index.isValid()) return nullptr;
    return reinterpret_cast<const p4cl::ast::Node*>(index.internalPointer());
}

int ProgramTreeModel::columnCount(const QModelIndex&) const
{
    return 2;
}

Qt::ItemFlags ProgramTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProgramTreeModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (section == 0 && role == Qt::DisplayRole)
        return "Object";
    else if (section == 1 && role == Qt::DisplayRole)
        return "Type/Value";

    return QVariant();
}

int ProgramTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!d->parseResult.rootNode) {
        return 0;
    }
    if (parent.column() > 0) return 0;

    const p4cl::ast::Node* parentNode;

    if (!parent.isValid())
        parentNode = d->parseResult.rootNode.get();
    else
        parentNode = reinterpret_cast<const p4cl::ast::Node*>(parent.internalPointer());

    if (!parentNode) {
        return 0;
    }

    return parentNode->children.size();
}

QVariant ProgramTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    const p4cl::ast::Node* node = static_cast<const p4cl::ast::Node*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole: {
            if (index.column() == 0)
                return ui::nodeName(node);
            else
                return ui::nodeType(node);
        }
        case Qt::DecorationRole:
            if (index.column() == 0) return ui::nodeIcon(node);
            break;
    }
    return QVariant();
}

QModelIndex ProgramTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    if (!d->parseResult.rootNode) {
        return QModelIndex();
    }

    const p4cl::ast::Node* parentNode;

    if (!parent.isValid())
        parentNode = d->parseResult.rootNode.get();
    else
        parentNode = reinterpret_cast<const p4cl::ast::Node*>(parent.internalPointer());

    if (!parentNode) {
        return QModelIndex();
    }

    const p4cl::ast::Node* childNode = parentNode->children[row];
    if (childNode)
        return createIndex(row, column, reinterpret_cast<quintptr>(childNode));
    else
        return QModelIndex();
}

QModelIndex ProgramTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    if (!d->parseResult.rootNode) {
        return QModelIndex();
    }

    const p4cl::ast::Node* childNode =
        reinterpret_cast<const p4cl::ast::Node*>(index.internalPointer());
    const p4cl::ast::Node* parentNode = childNode->parent;

    if (parentNode == d->parseResult.rootNode.get()) return QModelIndex();

    int distance = std::distance(
        parentNode->children.begin(),
        std::find(parentNode->children.begin(), parentNode->children.end(), childNode));
    return createIndex(distance, 0, reinterpret_cast<quintptr>(parentNode));
}
