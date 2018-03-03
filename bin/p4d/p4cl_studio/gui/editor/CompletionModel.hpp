#pragma once

#include "AssistItem.hpp"

#include <p4cl/parser/Parser.hpp>
#include <QAbstractListModel>

namespace p4s {

class CompletionModel : public QAbstractListModel {
public:
    CompletionModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    void updateAssistItems(std::vector<AssistItem>&&);    

public:
    std::vector<AssistItem> assistItems;
    int rowHeight = 25;

};
}
