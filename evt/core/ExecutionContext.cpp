#include "ExecutionContext.hpp"

evt::ExecutionState::ExecutionState()
{
    _data = std::make_shared<Data>();
}

evt::ExecutionState::ExecutionState(std::shared_ptr<evt::ExecutionContextData> cd)
    : ExecutionState()
{
    _data->exectutionContextData = std::move(cd);
}

evt::ExecutionState evt::ExecutionState::push()
{
    ExecutionState s{_data->exectutionContextData};
    _data->children->push_back(s);
    if (_data->exectutionContextData->changeObserver) {
        _data->exectutionContextData->changeObserver(*this, TaskStatusChange::Hierarchy);
    }

    return s;
}

void evt::ExecutionState::log(const evt::LogMessage& m)
{
    _data->children->push_back(m);
    if (_data->exectutionContextData->changeObserver) {
        _data->exectutionContextData->changeObserver(*this, TaskStatusChange::Hierarchy);
    }
}

void evt::ExecutionState::setProgress(uint8_t p)
{
    if (p != _data->progress) {
        _data->progress = p;
        if (_data->exectutionContextData->changeObserver) {
            _data->exectutionContextData->changeObserver(*this, TaskStatusChange::Progress);
        }
    }
}

void evt::ExecutionState::setStatus(evt::ExecutionStatus s)
{
    if (s != _data->status) {
        _data->status = s;
        if (_data->exectutionContextData->changeObserver) {
            _data->exectutionContextData->changeObserver(*this, TaskStatusChange::Status);
        }
    }
}

const ev::synchronized_data<evt::ExecutionState::SubTasks>& evt::ExecutionState::subTasks() const
{
    return _data->children;
}

evt::ExecutionStatus evt::ExecutionState::status() const
{
    return _data->status;
}

uint8_t evt::ExecutionState::progress() const
{
    return _data->progress;
}

const void* evt::ExecutionState::dataPtr() const
{
    return _data.get();
}

evt::ExecutionContext::ExecutionContext()
{
    _data = std::make_shared<ExecutionContextData>();
    _data->rootTaskStatus._data->exectutionContextData = _data;
}

void evt::ExecutionContext::setChangeObserver(evt::TaskStatusObserver obs)
{
    _data->changeObserver = std::move(obs);
}

evt::ExecutionState& evt::ExecutionContext::rootTaskStatus()
{
    return _data->rootTaskStatus;
}

void evt::ExecutionContext::stop()
{
    _data->stopRequest = true;
}

void evt::ExecutionContext::wait()
{
    _data->done.get_future().get();
}

void evt::ExecutionContext::log(const evt::LogMessage& m)
{
    rootTaskStatus().log(m);
}

void evt::ExecutionContext::setProgress(uint8_t p)
{
    rootTaskStatus().setProgress(p);
}

void evt::ExecutionContext::setStatus(evt::ExecutionStatus s)
{
    rootTaskStatus().setStatus(s);
    if (s == ExecutionStatus::Finished || s == ExecutionStatus::Error ||
        s == ExecutionStatus::Stopped)
        _data->done.set_value();
}

bool evt::ExecutionContext::shouldStop() const
{
    return _data->stopRequest;
}

void evt::ExecutionContext::hardExit()
{
    throw ExectutionStopException();
}
