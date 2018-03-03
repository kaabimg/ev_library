#include "OutputPaneWidget.hpp"
#include "ExecutionStateWidget.hpp"
#include "Console.hpp"
#include "Application.hpp"

evt::OutputPaneWidget::OutputPaneWidget(QWidget* parent) : TabWidget(parent)
{
    _executionStateWidget = new ExecutionStateWidget(this);
    _executionStateWidget->setRoot(app()->mainExecutionState());

    connect(app(), &Application::executionStateChanged, _executionStateWidget,
            &ExecutionStateWidget::sync);

    _consoleWidget = new Console(this);
    addTab(_executionStateWidget, "Messages");
    addTab(_consoleWidget, "Console");
}
