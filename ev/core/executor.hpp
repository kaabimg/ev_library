#pragma once

#include "preprocessor.hpp"
#include "thread.hpp"

#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <thread>

namespace ev {
class executor {
    template <typename F, typename... Args>
    using result_of = typename std::result_of_t<std::decay_t<F>(std::decay_t<Args>...)>;

public:
    executor(size_t thread_count = std::max(1u, boost::thread::hardware_concurrency()));

    size_t thread_count() const;

    template <typename F, typename... Args>
    inline future<result_of<F, Args...>> async(F&& f, Args&&... args);

    template <typename F, typename... Args>
    inline void async_detached(F&& f, Args&&... args);

    template <class F>
    inline executor& operator<<(F&& f);

    ~executor();

protected:
    void work();
    void join_all();

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queue_mutex;
    std::condition_variable m_wait_condition;
    bool m_done = false;
};

// the constructor just launches some amount of workers
inline executor::executor(size_t thread_count)
{
    for (size_t i = 0; i < thread_count; ++i) m_threads.emplace_back([this] { this->work(); });
}

inline size_t executor::thread_count() const
{
    return m_threads.size();
}

inline void executor::work()
{
    while (!m_done) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_wait_condition.wait(lock, [this] { return m_done || !m_tasks.empty(); });
            if (m_done) return;
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}

template <typename F, typename... Args>
inline future<executor::result_of<F, Args...>> executor::async(F&& f, Args&&... args)
{
    using return_type = result_of<F, Args...>;

    packaged_task<return_type> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    future<return_type> res = task.get_future();
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        m_tasks.emplace([t = std::move(task)]() { t(); });
    }
    m_wait_condition.notify_one();
    return res;
}

template <typename F, typename... Args>
inline void executor::async_detached(F&& f, Args&&... args)
{
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        m_tasks.emplace(std::move(func));
    }
    m_wait_condition.notify_one();
}

template <class F>
inline executor& executor::operator<<(F&& f)
{
    async_detached(std::forward<F>(f));
    return *this;
}

inline void executor::join_all()
{
    for (auto& worker : m_threads) worker.join();
}

inline executor::~executor()
{
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        m_done = true;
        m_wait_condition.notify_all();
    }
    join_all();
}

}  // ev
