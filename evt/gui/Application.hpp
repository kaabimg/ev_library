#pragma once

#include <ev/core/pimpl.hpp>

#include <evt/core/ApplicationScriptEngine.hpp>
#include <evt/core/ExecutionContext.hpp>
#include <future>

#include <QApplication>

namespace evt {
class Style;
class ApplicationScriptEngine;

class ExecutionEvent : public QEvent {
public:
    enum EventType { Execution = QEvent::User + 1 };
    ExecutionEvent() : QEvent(Type(Execution))
    {
    }
    std::function<void()> task;
};

class Application : public QApplication {
    Q_OBJECT

    template <typename F, typename... Args>
    using result_of = typename std::result_of_t<std::decay_t<F>(std::decay_t<Args>...)>;

public:
    Application(int& argc, char** argv, int = ApplicationFlags);
    ~Application();
    ApplicationScriptEngine& scriptEngine();
    ExecutionState& mainExecutionState();

    template <typename F, typename... Args>
    inline std::future<result_of<F, Args...>> async(F&& f, Args&&... args);

public Q_SLOTS:
    void setTheme(const QString&);

Q_SIGNALS:
    void executionStateChanged(ExecutionState, TaskStatusChange);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    struct Impl;
    ev::pimpl<Impl> d;
};

template <typename F, typename... Args>
inline std::future<Application::result_of<F, Args...>> Application::async(F&& f, Args&&... args)
{
    using return_type = result_of<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    ExecutionEvent* event = new ExecutionEvent;
    event->task = [task]() { (*task)(); };

    postEvent(this, event);

    return task->get_future();
}

inline Application* app()
{
    return reinterpret_cast<Application*>(QCoreApplication::instance());
}
}
