#pragma once

#include <p4cl/P4CLang.hpp>
#include <memory>
#include <QAbstractListModel>

namespace p4s {

class CompilationLogModel : public QAbstractListModel
{
public:
    CompilationLogModel(QObject* parent = nullptr);

    void setLog(std::shared_ptr<p4cl::lang::CompilationLog> log);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;


    p4cl::TaggedPosition postionOf(const QModelIndex& index)const;

    std::shared_ptr<p4cl::lang::CompilationLog> log;
};

}
