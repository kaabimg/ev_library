#pragma once

#include <evt/gui/TabWidget.hpp>

namespace evt {

class ExecutionStateWidget;
class Console;
class Style;

class OutputPaneWidget : public TabWidget {
    Q_OBJECT
public:
    OutputPaneWidget(QWidget* parent = nullptr);

private:
    ExecutionStateWidget* _executionStateWidget;
    Console* _consoleWidget;
};
}
