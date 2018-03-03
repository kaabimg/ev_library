#pragma once

#include <ev/core/pimpl.hpp>

#include <evt/core/ApplicationScriptEngine.hpp>
#include <evt/core/ExecutionContext.hpp>

#include <QApplication>

namespace evt {
class Style;
class ApplicationScriptEngine;

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int& argc, char** argv, int = ApplicationFlags);
    ~Application();
    ApplicationScriptEngine& scriptEngine();
    ExecutionState& mainExecutionState();

public Q_SLOTS:
    void setTheme(const QString&);

Q_SIGNALS:
    void executionStateChanged(ExecutionState, TaskStatusChange);

private:
    struct Impl;
    ev::pimpl<Impl> d;
};

inline Application* app()
{
    return reinterpret_cast<Application*>(QCoreApplication::instance());
}

}
