#include "ExecutionStateWidget.hpp"
#include "Style.hpp"

#include "../style/stylesheets/TreeView.hpp"

#include <QTreeWidget>
#include <QVBoxLayout>

using namespace evt;
struct ExecutionStateWidget::Impl {
    QTreeWidget* output;
    std::unordered_map<const void*, QTreeWidgetItem*> stateMap;
};

evt::ExecutionStateWidget::ExecutionStateWidget(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    d->output = new QTreeWidget(this);
    d->output->setHeaderHidden(true);

    layout->addWidget(d->output);

    monitorStyleChangeFor(this);
    applyStyle(appStyle());
}

ExecutionStateWidget::~ExecutionStateWidget()
{
}

void ExecutionStateWidget::setRoot(const ExecutionState& root)
{
    auto rootNode = new QTreeWidgetItem(d->output);
    d->stateMap[root.dataPtr()] = rootNode;
    d->output->insertTopLevelItem(0, rootNode);
}

void evt::ExecutionStateWidget::applyStyle(const evt::Style& style)
{
    d->output->setStyleSheet(style.adaptStyleSheet(treeViewStyleSheet));
}

QTreeWidgetItem* evt::ExecutionStateWidget::makeMessageNode(const LogMessage& msg)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(d->output);
    item->setText(0, msg.msg.c_str());
    return item;
}
QTreeWidgetItem* evt::ExecutionStateWidget::makeTaskNode(const ExecutionState& /*state*/)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(d->output);
    item->setText(0, "todo");
    return item;
}
void evt::ExecutionStateWidget::sync(const ExecutionState& execState, TaskStatusChange change)
{
    switch (change) {
        case TaskStatusChange::Status: {
            break;
        }
        case TaskStatusChange::Progress: {
            break;
        }
        case TaskStatusChange::Hierarchy: {
            auto iter = d->stateMap.find(execState.dataPtr());
            QTreeWidgetItem* parent;
            if (iter != d->stateMap.end())  // new node
                parent = iter->second;
            else
                parent = d->output->topLevelItem(0);

            QTreeWidgetItem* node;
            const void* newTaskPt = nullptr;

            ev_synchronized(tasks, execState.subTasks())
            {
                auto& lastTask = tasks[parent->childCount()];
                if (std::holds_alternative<LogMessage>(lastTask)) {
                    node = makeMessageNode(std::get<LogMessage>(lastTask));
                }
                else {
                    node = makeTaskNode(std::get<ExecutionState>(lastTask));
                    newTaskPt = std::get<ExecutionState>(lastTask).dataPtr();
                }
            }

            parent->addChild(node);
            if (newTaskPt) d->stateMap[newTaskPt] = node;

            break;
        }
        default: break;
    }
}
