#pragma once

#include <evt/core/ExecutionContext.hpp>
#include <ev/core/pimpl.hpp>
#include <QWidget>

class QTreeWidgetItem;

namespace evt {
class Style;
class ExecutionState;
class ExecutionStateWidget : public QWidget {
    Q_OBJECT
public:
    explicit ExecutionStateWidget(QWidget* parent = nullptr);
    ~ExecutionStateWidget();

    void setRoot(const ExecutionState&);

    void applyStyle(const Style&);

public Q_SLOTS:
    void sync(const ExecutionState&, TaskStatusChange change);

private:
    QTreeWidgetItem* makeMessageNode(const LogMessage& msh);
    QTreeWidgetItem* makeTaskNode(const ExecutionState& state);

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
