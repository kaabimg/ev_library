#ifndef EV_EVENT_QUEUE_H
#define EV_EVENT_QUEUE_H

#include "macros.h"
#include "types.h"

#include <atomic>
#include <functional>

namespace ev { namespace core {

struct event_flags_e   {
    enum value {
        no_flags             = 0,
        delete_after_process = 1
    };
};


struct standard_event_type_e {
    enum value {
        callable_event = -1,
    };

};

static const int32 unknown_event_type = 0;

struct event_t {
    virtual ~event_t(){}

    int32 type = unknown_event_type; // event type must be > unknown_event_type ( == 0)
    uint32 flags = event_flags_e::no_flags;
    std::atomic_bool done {false};


    bool is_standard_event()const{
        return type < 0;
    }

    template <typename event>
    const event* as()const{
        return reinterpret_cast<const event*>(this);
    }

    template <typename event>
    event* as(){
        return reinterpret_cast<event*>(this);
    }
};


template <typename event>
inline event* make_event(event_flags_e flags){
    event* e = new event;
    e->flags = flags;
    return e;
}




struct callable_event_t  : event_t {
    callable_event_t(const std::function<void ()>& f_):event_t(),f(f_){
        type = standard_event_type_e::callable_event;
    }

    callable_event_t(std::function<void ()> && f_):event_t(),f(std::move(f_)){
        type = standard_event_type_e::callable_event;
    }

    std::function<void ()> f;


    static callable_event_t*  make (const std::function<void ()>& f){
        callable_event_t * e = new callable_event_t(f);
        return e;
    }

    static callable_event_t*  make (std::function<void ()>&& f){
        callable_event_t * e = new callable_event_t(std::move(f));
        return e;
    }
};




struct event_processor_t
{
    event_processor_t(){}
    virtual ~event_processor_t(){}
    virtual void process(event_t*)=0;
};


struct standard_event_processor_t {
    static void process(event_t*);
};

struct event_dispatcher_t  {

    event_dispatcher_t();
    ~event_dispatcher_t();
    void process(event_t*) ;
    void add_event_processor(event_processor_t*);

private:
    EV_PRIVATE(event_dispatcher_t)

    static void work(event_processor_t* worker,data_t* d);

};

}}



#endif // EV_EVENT_QUEUE_H
