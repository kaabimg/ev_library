#ifndef EV_THREAD_POOL_H
#define EV_THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "preprocessor.h"

namespace ev {

class thread_pool_t {
public:
    thread_pool_t(size_t size = std::max(1u,std::thread::hardware_concurrency()));

    template<class F, class... Args>
    inline auto post(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;


    template<class F, class... Args>
    inline void post_detached(F&& f, Args&&... args);

    inline void stop();

    ~thread_pool_t();

protected:
    void work();
    void join_all();

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queue_mutex;
    std::condition_variable m_wait_condition;
    bool m_stop;
};

// the constructor just launches some amount of workers
inline thread_pool_t::thread_pool_t(size_t threads)
    :   m_stop(false)
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
                [this]{ return m_stop || !m_tasks.empty(); });
            if(m_stop && m_tasks.empty())
                return;
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}

void thread_pool_t::stop()
{
    m_stop = true;
}


template<class F, class... Args>
auto thread_pool_t::post(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    if(m_stop) throw std::runtime_error("posting on stopped thread pool");

    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
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

template<class F, class... Args>
void thread_pool_t::post_detached(F&& f, Args&&... args)
{
    if(m_stop) throw std::runtime_error("posting on stopped thread pool");
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        ev_unused(lock);
        std::function<void(Args &&... args)> func{std::forward<F>(f),std::forward<Args>(args)...};
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
    stop();
    m_wait_condition.notify_all();
    join_all();
}



} // ev


#endif // EV_THREAD_POOL_H
