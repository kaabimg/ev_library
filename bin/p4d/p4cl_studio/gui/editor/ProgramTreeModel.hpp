#pragma once

#include <ev/core/pimpl.hpp>

#include <QAbstractItemModel>

namespace p4cl {
namespace parser {
class Result;
}
namespace ast {
class Node;
}
}

namespace p4s {

class ProgramTreeModel : public QAbstractItemModel {
    Q_OBJECT
public:
    ProgramTreeModel(QObject* parent = 0);
    ~ProgramTreeModel();

    void setRoot(p4cl::parser::Result);

    const p4cl::ast::Node* nodeAt(const QModelIndex&)const;

    // model api
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

private:
    class Impl;
    ev::pimpl<Impl> _impl;
};
}
