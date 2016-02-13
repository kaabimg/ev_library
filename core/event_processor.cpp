#include "event_processor.h"

using namespace ev::core;

#include "event_processor.h"

#include <boost/lockfree/queue.hpp>


#include <condition_variable>
#include <thread>

using namespace ev;

using event_queue_t = boost::lockfree::queue<event_t*>;

struct event_dispatcher_t::data_t {
    event_queue_t events {128};
    std::condition_variable synchronizer;
    std::mutex synchronization_mutex;

    std::atomic_int threads_count {0};
    std::atomic_bool exit {false};
};

namespace details {


struct thread_count_handler {
    std::atomic_int &  count;
    thread_count_handler(std::atomic_int & c):count(c){
        count++;
    }

    ~thread_count_handler () {
        count --;
    }
};

}


event_dispatcher_t::event_dispatcher_t():d(new data_t){}


event_dispatcher_t::~event_dispatcher_t()
{
    d->exit = true;
    d->synchronizer.notify_all();
    while (d->threads_count) {}

    delete d;
}

void event_dispatcher_t::process(event_t* e)
{
    while(!d->events.push(e)){};
    d->synchronizer.notify_one();
}


void event_dispatcher_t::work(event_processor_t* worker,event_dispatcher_t::data_t* d)
{
    details::thread_count_handler tch(d->threads_count);
    ev_unused(tch);

    event_t* e;

    ev_forever {
        while(d->events.pop(e)){
            if(e->is_standard_event()){
                standard_event_processor_t::process(e);
            }
            else {
                worker->process(e);
            }

        }

        if(d->exit){
            while(d->events.pop(e)){
                worker->process(e);
            }
            return;
        }
        else {
            std::unique_lock<std::mutex> lock_guard {d->synchronization_mutex};
            d->synchronizer.wait(lock_guard);
        }
    }
}


void event_dispatcher_t::add_event_processor(event_processor_t* worker)
{
    std::thread thread {&work,worker,d};
    thread.detach();

}


void standard_event_processor_t::process(event_t* e)
{
    switch (e->type) {
    case standard_event_type_e::callable_event:
        e->as<callable_event_t>()->f();

        break;
    default:
        break;
    }
}

