#pragma once

#include "preprocessor.hpp"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

namespace ev {
class executor_t {
    template <typename F, typename... Args>
    using result_of = typename std::result_of_t<std::decay_t<F>(std::decay_t<Args>...)>;

public:
    executor_t(size_t thread_count = std::max(1u, std::thread::hardware_concurrency()));

    size_t thread_count() const;

    template <typename F, typename... Args>
    inline std::future<result_of<F, Args...>> async(F&& f, Args&&... args);

    template <typename F, typename... Args>
    inline void async_detached(F&& f, Args&&... args);

    template <class F>
    inline executor_t& operator<<(F&& f);

    ~executor_t();

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
inline executor_t::executor_t(size_t thread_count)
{
    for (size_t i = 0; i < thread_count; ++i) m_threads.emplace_back([this] { this->work(); });
}

inline size_t executor_t::thread_count() const
{
    return m_threads.size();
}

inline void executor_t::work()
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
inline std::future<executor_t::result_of<F, Args...>> executor_t::async(F&& f, Args&&... args)
{
    using return_type = result_of<F, Args...>;
    using task_type = std::packaged_task<return_type()>;

    auto task =
        std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        m_tasks.emplace([task]() { (*task)(); });
    }
    m_wait_condition.notify_one();
    return res;
}

template <typename F, typename... Args>
inline void executor_t::async_detached(F&& f, Args&&... args)
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
inline executor_t& executor_t::operator<<(F&& f)
{
    async_detached(std::forward<F>(f));
    return *this;
}

inline void executor_t::join_all()
{
    for (std::thread& worker : m_threads) worker.join();
}

inline executor_t::~executor_t()
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
