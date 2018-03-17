#include "Application.hpp"
#include "Style.hpp"
#include "../core/ApplicationScriptEngine.hpp"
#include "../core/ExecutionContext.hpp"

using namespace evt;

struct Application::Impl {
    ApplicationScriptEngine scriptEngine;
    ExecutionContext executionContext;
};

Application::Application(int& argc, char** argv, int flags) : QApplication(argc, argv, flags), d()
{
    Style::changeNotifier = new ChangeNotifier(this);
    d->executionContext.setChangeObserver(
        [this](ExecutionState es, TaskStatusChange c) { Q_EMIT executionStateChanged(es, c); });

    installEventFilter(this);
}

Application::~Application()
{
}

void Application::setTheme(const QString&)
{
}

ApplicationScriptEngine& Application::scriptEngine()
{
    return d->scriptEngine;
}

ExecutionState& Application::mainExecutionState()
{
    return d->executionContext.rootTaskStatus();
}

bool Application::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this && int(event->type()) == ExecutionEvent::Execution) {
        ExecutionEvent* execEvent = static_cast<ExecutionEvent*>(event);
        execEvent->task();
        return true;
    }
    else
        return false;
}
