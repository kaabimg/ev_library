#pragma once

#include "preprocessor.hpp"

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <condition_variable>
#include <future>
#include <functional>


namespace ev {

class thread_pool_t {
public:
    thread_pool_t(size_t size = std::max(1u,std::thread::hardware_concurrency()));

    template<class F>
    inline std::future<typename std::result_of<F()>::type> post(F&& f);

    template<class F>
    inline void post_detached(F&& f);

    ~thread_pool_t();

protected:
    void work();
    void join_all();

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queue_mutex;
    std::condition_variable m_wait_condition;
};

// the constructor just launches some amount of workers
inline thread_pool_t::thread_pool_t(size_t threads)
{
    for(size_t i = 0;i<threads;++i)
        m_threads.emplace_back([this]{this->work();});
}


void thread_pool_t::work()
{
    for(;;)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_wait_condition.wait(lock,
                [this]{ return !m_tasks.empty(); });
            if(m_tasks.empty())
                return;
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}


template<class F>
std::future<typename std::result_of<F()>::type> thread_pool_t::post(F&& f)
{
    using return_type = typename std::result_of<F()>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f))
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        m_tasks.emplace([task](){ (*task)(); });
    }
    m_wait_condition.notify_one();
    return res;
}

template<class F>
void thread_pool_t::post_detached(F&& f)
{
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        std::function<void()> func{std::forward<F>(f)};
        m_tasks.emplace(std::move(func));
    }
    m_wait_condition.notify_one();
}


void thread_pool_t::join_all()
{
    for(std::thread &worker: m_threads) worker.join();
}


thread_pool_t::~thread_pool_t()
{
    m_wait_condition.notify_all();
    join_all();
}



} // ev

