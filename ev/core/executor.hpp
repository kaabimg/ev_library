#pragma once

#include "thread.hpp"

#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>

namespace ev {
class executor {
    template <typename F, typename... Args>
    using result_of = typename std::result_of_t<std::decay_t<F>(std::decay_t<Args>...)>;

public:
    executor(size_t thread_count = std::max(1u, boost::thread::hardware_concurrency()));

    size_t thread_count() const;

    void wait();

    template <typename F, typename... Args>
    inline future<result_of<F, Args...>> async(F&& f, Args&&... args);

    template <typename F, typename... Args>
    inline void async_detached(F&& f, Args&&... args);

    template <class F>
    inline executor& operator<<(F&& f);

    ~executor();

protected:
    void join_all();
    template <typename F>
    void dispatch(F&& f);

private:
    using task_type = std::function<void()>;

    struct execution_queue {
        std::queue<task_type> tasks;
        std::mutex queue_mutex;
        std::condition_variable wait_condition;
        std::atomic_uint job_count{0};
        bool done = false;
        std::thread thread;

        execution_queue();
        void push(task_type&& task);
        void work();
        void sync(latch& l);
        void stop();
    };

    std::vector<execution_queue> _execution_queues;
};

executor::execution_queue::execution_queue() : thread([this] { work(); })
{
}

inline void executor::execution_queue::push(executor::task_type&& task)
{
    {
        std::lock_guard lock(queue_mutex);
        tasks.push(std::move(task));
    }
    ++job_count;
    wait_condition.notify_one();
}

inline void executor::execution_queue::work()
{
    while (!done) {
        task_type task;
        {
            std::unique_lock lock(queue_mutex);
            wait_condition.wait(lock, [this] { return done || !tasks.empty(); });
            if (done) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
        --job_count;
    }
}

inline void executor::execution_queue::sync(latch& l)
{
    push([&]() { l.count_down(); });
}

inline void executor::execution_queue::stop()
{
    {
        std::lock_guard lock(queue_mutex);
        done = true;
    }
    wait_condition.notify_one();
}

/////////////////////////////////////

inline executor::executor(size_t thread_count) : _execution_queues(thread_count)
{
}

inline size_t executor::thread_count() const
{
    return _execution_queues.size();
}

template <typename F>
void executor::dispatch(F&& f)
{
    std::min_element(_execution_queues.begin(), _execution_queues.end(), [](auto&& l, auto&& r) {
        return l.job_count < r.job_count;
    })->push(std::forward<F>(f));
}

template <typename F, typename... Args>
inline future<executor::result_of<F, Args...>> executor::async(F&& f, Args&&... args)
{
    using return_type = result_of<F, Args...>;

    auto task = std::make_shared<packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    dispatch([task]() mutable { (*task)(); });

    return task->get_future();
}

template <typename F, typename... Args>
inline void executor::async_detached(F&& f, Args&&... args)
{
    dispatch(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
}

template <class F>
inline executor& executor::operator<<(F&& f)
{
    async_detached(std::forward<F>(f));
    return *this;
}

inline void executor::join_all()
{
    for (auto& q : _execution_queues) q.thread.join();
}

inline void executor::wait()
{
    latch sync_latch{uint(thread_count())};
    for (auto& q : _execution_queues) q.sync(sync_latch);
    while (!sync_latch.try_wait()) {
    }
}

inline executor::~executor()
{
    for (auto& q : _execution_queues) q.stop();
    join_all();
}

}  // ev
