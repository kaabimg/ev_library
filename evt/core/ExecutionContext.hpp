#pragma once

#include <evt/core/Logging.hpp>

#include <ev/core/synchronized_data.hpp>

#include <functional>
#include <future>
#include <vector>
#include <variant>

namespace evt {

enum class ExecutionStatus { Pending, Running, Stopped, Finished, Error };

inline const char* toString(ExecutionStatus s)
{
    switch (s) {
        case ExecutionStatus::Pending: return "Pending";
        case ExecutionStatus::Running: return "Running";
        case ExecutionStatus::Stopped: return "Stopped";
        case ExecutionStatus::Finished: return "Finished";
        case ExecutionStatus::Error: return "Error";
    }
}

enum TaskStatusChange { Status, Progress, Hierarchy };

class ExectutionStopException : std::exception {
public:
    ~ExectutionStopException() noexcept
    {
    }
    const char* what() const noexcept
    {
        return "Task stopped";
    }
};

class ExecutionContextData;

class ExecutionState {
public:
    using SubTaskData = std::variant<LogMessage, ExecutionState>;
    using SubTasks = std::vector<SubTaskData>;

    ExecutionState();
    ExecutionState(std::shared_ptr<ExecutionContextData>);

    ExecutionState push();

    void log(const LogMessage& m);
    void setProgress(uint8_t p);
    void setStatus(ExecutionStatus s);

    const ev::synchronized_data<SubTasks>& subTasks() const;
    ExecutionStatus status() const;
    uint8_t progress() const;

    const void* dataPtr()const;

private:
    struct Data {
        ev::synchronized_data<SubTasks> children;
        std::shared_ptr<ExecutionContextData> exectutionContextData;
        ExecutionStatus status = ExecutionStatus::Pending;
        uint8_t progress = 0;
    };

private:
    std::shared_ptr<Data> _data;
    friend class ExecutionContext;
};

using TaskStatusObserver = std::function<void(ExecutionState, TaskStatusChange)>;

struct ExecutionContextData {
    ExecutionState rootTaskStatus;
    TaskStatusObserver changeObserver;
    bool stopRequest = false;
    std::promise<void> done;
};

class ExecutionContext {
public:
    ExecutionContext();

    /// Controller API
    void setChangeObserver(TaskStatusObserver obs);
    ExecutionState& rootTaskStatus();
    void stop();
    void wait();

    /// Task API
    void log(const LogMessage& m);
    void setProgress(uint8_t p);
    void setStatus(ExecutionStatus s);
    bool shouldStop() const;
    void hardExit();

private:
    std::shared_ptr<ExecutionContextData> _data;
};
}
