#include "CompletionModel.hpp"

#include <evt/gui/Style.hpp>

using namespace p4s;

CompletionModel::CompletionModel(QObject* parent) : QAbstractListModel(parent)
{
}

QVariant CompletionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (index.row() >= assistItems.size() || index.row() < 0) return QVariant();

    if (role == Qt::DisplayRole) {
        return assistItems.at(index.row()).label;
    }
    if (role == Qt::DecorationRole) {
        return assistItems.at(index.row()).icon;
    }
    if (role == Qt::BackgroundRole) {
        return evt::appStyle().theme.background.normal;
    }
    if (role == Qt::ForegroundRole) {
        return evt::appStyle().theme.foregroundColor;
    }
    if (role == Qt::EditRole) {
        return index.row();
    }
    if (role == Qt::SizeHintRole) {
        return QSize(50, rowHeight);
    }
    return QVariant();
}

int CompletionModel::rowCount(const QModelIndex& /*parent*/) const
{
    return assistItems.size();
}

void CompletionModel::updateAssistItems(std::vector<AssistItem>&& items)
{
    beginResetModel();
    assistItems = std::move(items);
    endResetModel();
}
