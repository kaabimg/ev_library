#include "CompilationLogModel.hpp"
#include "../UiUtils.hpp"
#include <evt/gui/Style.hpp>

using namespace p4s;

CompilationLogModel::CompilationLogModel(QObject* parent) : QAbstractListModel(parent)
{
}

void CompilationLogModel::setLog(std::shared_ptr<p4cl::lang::CompilationLog> l)
{
    beginResetModel();
    log = l;
    endResetModel();
}

int CompilationLogModel::rowCount(const QModelIndex&) const
{
    if (log) return log->size();
    return 0;
}

p4cl::TaggedPosition CompilationLogModel::postionOf(const QModelIndex& index) const
{
    return log->at(index.row()).position;
}

QVariant CompilationLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (index.row() >= log->size() || index.row() < 0) return QVariant();

    using p4cl::lang::CompilationMessage;

    auto& msg = log->at(index.row());

    if (role == Qt::DisplayRole) {
        return msg.text.c_str();
    }
    if (role == Qt::BackgroundRole) {
        return evt::appStyle().theme.background.normal;
    }
    if (role == Qt::DecorationRole) {
        switch (msg.category) {
            case p4cl::lang::CompilationMessage::Warning:
                return evt::appStyle().icon(evt::IconType::Warning);
            case p4cl::lang::CompilationMessage::CompilationError:
            case p4cl::lang::CompilationMessage::EvaluationError:
            case p4cl::lang::CompilationMessage::ParsingError:
                return evt::appStyle().icon(evt::IconType::Error);
            default: return QVariant();
        }
    }
    return QVariant();
}
