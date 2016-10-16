#ifndef EV_SHARED_DATA_H
#define EV_SHARED_DATA_H

#include "preprocessor.h"

#include <shared_mutex>


namespace ev {

struct exclusive_lock_trait_t {
    using mutex_t = std::mutex;
    static inline void lock_function(mutex_t & m){
        m.lock();
    }
    static inline void unlock_function(mutex_t& m){
        m.unlock();
    }
};



template <typename T,typename LockTrait = exclusive_lock_trait_t>
class synchronized_data_t {
    T m_data;
    mutable typename LockTrait::mutex_t m_mutex;
public:

    template <void (lock_function)(typename LockTrait::mutex_t&),
              void (unlock_function)(typename LockTrait::mutex_t&)>
    class lock_t {
        synchronized_data_t* m_data;

    public:
        inline lock_t(synchronized_data_t * d):m_data(d){
            if(m_data) lock_function(m_data->m_mutex);
        }

        inline lock_t(const lock_t & rhs):lock_t(rhs.m_data){}
        inline lock_t(lock_t && rhs){
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            if(m_data) lock_function(m_data->m_mutex);
        }

        inline ~lock_t(){
            if(m_data) unlock_function(m_data->m_mutex);
        }


        inline lock_t& operator =(const lock_t & rhs){
            if(m_data) unlock_function(m_data->m_mutex);
            m_data = rhs.m_data;
            if(m_data) lock_function(m_data->m_mutex);
            return *this;
        }

        inline lock_t& operator =(lock_t && rhs){
            if(m_data) unlock_function(m_data->m_mutex);
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            return *this;
        }

        inline T* operator->(){
            return &m_data->m_data;
        }
    };


    /// synchronized_data_t

    using exclusive_lock_t = lock_t<LockTrait::lock_function,LockTrait::unlock_function>;

    synchronized_data_t(){}

    synchronized_data_t(const synchronized_data_t & rhs){
        auto read_lock = rhs.acquire_exclusive_lock();
        ev_unused(read_lock);
        m_data = rhs.m_data;
    }

    synchronized_data_t(synchronized_data_t && rhs):m_data(std::move(rhs.m_data)){}


    synchronized_data_t & operator = (const synchronized_data_t & rhs){

        if(this < &rhs){
            auto wl = acquire_exclusive_lock();
            auto rl = rhs.acquire_shared_lock();

            ev_unused(wl);
            ev_unused(rl);

            m_data = rhs.m_data;
        }

        else if(this> &rhs){
            auto rl = rhs.acquire_shared_lock();
            auto wl = acquire_exclusive_lock();

            ev_unused(wl);
            ev_unused(rl);

            m_data = rhs.m_data;

        }

        return *this;
    }


    synchronized_data_t & operator = (synchronized_data_t && rhs){
        auto lock = acquire_exclusive_lock();
        ev_unused(lock);
        m_data = std::move(rhs);
        return *this;
    }

    inline exclusive_lock_t acquire_exclusive_lock(){
        return exclusive_lock_t(this);
    }

    inline exclusive_lock_t operator->(){
        return acquire_exclusive_lock();
    }

};



struct shared_lock_trait_t {
    using mutex_t = std::shared_mutex;

    static inline void shared_lock_function(mutex_t & m){
        m.lock_shared();
    }

    static inline void shared_unlock_function(mutex_t& m){
        m.unlock_shared();
    }

    static inline void lock_function(mutex_t & m){
        m.lock();
    }

    static inline void unlock_function(mutex_t& m){
        m.unlock_shared();
    }
};



template <typename T,typename LockTrait = shared_lock_trait_t>
class shared_data_t {
    T m_data;
    mutable typename LockTrait::mutex_t m_mutex;

public:

    template <typename TA,
              void (lock_function)(typename LockTrait::mutex_t&),
              void (unlock_function)(typename LockTrait::mutex_t&)>
    class lock_t {
        shared_data_t* m_data;

    public:
        inline lock_t(shared_data_t * d):m_data(d){
            if(m_data) lock_function(m_data->m_mutex);
        }

        inline lock_t(const lock_t & rhs):lock_t(rhs.m_data){}
        inline lock_t(lock_t && rhs){
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            if(m_data) lock_function(m_data->m_mutex);
        }

        inline ~lock_t(){
            if(m_data) unlock_function(m_data->m_mutex);
        }


        inline lock_t& operator =(const lock_t & rhs){
            if(m_data) unlock_function(m_data->m_mutex);
            m_data = rhs.m_data;
            if(m_data) lock_function(m_data->m_mutex);
            return *this;
        }

        inline lock_t& operator =(lock_t && rhs){
            if(m_data) unlock_function(m_data->m_mutex);
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            return *this;
        }

        inline TA* operator->(){
            return &m_data->m_data;
        }
    };


    /// shared_data_t

    using shared_lock_t = lock_t<const T,LockTrait::shared_lock_function,LockTrait::shared_unlock_function>;
    using exclusive_lock_t = lock_t<T,LockTrait::lock_function,LockTrait::unlock_function>;

    shared_data_t(){}

    shared_data_t(const shared_data_t & rhs){
        auto read_lock = rhs.acquire_shared_lock();
        ev_unused(read_lock);
        m_data = rhs.m_data;
    }

    shared_data_t(shared_data_t && rhs):m_data(std::move(rhs.m_data)){}


    shared_data_t & operator = (const shared_data_t & rhs){

        if(this < &rhs){
            auto wl = acquire_exclusive_lock();
            auto rl = rhs.acquire_shared_lock();
            ev_unused(wl);ev_unused(rl);
            m_data = rhs.m_data;
        }

        else if(this> &rhs){
            auto rl = rhs.acquire_shared_lock();
            auto wl = acquire_exclusive_lock();
            ev_unused(wl);ev_unused(rl);
            m_data = rhs.m_data;
        }
        return *this;
    }


    shared_data_t & operator = (shared_data_t && rhs){
        auto lock = acquire_exclusive_lock();
        ev_unused(lock);
        m_data = std::move(rhs);
        return *this;
    }


    inline const shared_lock_t acquire_shared_lock()const{
        return shared_lock_t(const_cast<shared_data_t*>(this));
    }

    inline exclusive_lock_t acquire_exclusive_lock(){
        return exclusive_lock_t(this);
    }

    inline shared_lock_t operator->()const{
        return acquire_shared_lock();
    }

    inline exclusive_lock_t operator->(){
        return acquire_exclusive_lock();
    }

    inline const shared_data_t& as_const()const{
        return *this;
    }
};



} //ev


// ev_shared_block
#define ev_shared_block(var) \
    if(bool _ev_shared_block_stop = false) {}\
    else for(auto _ev_shared_block_lock = var.acquire_shared_lock();!_ev_shared_block_stop;) \
            for(const auto & var = *_ev_shared_block_lock.operator->();!_ev_shared_block_stop;_ev_shared_block_stop=true)

#ifndef shared_block
#define shared_block ev_shared_block
#endif



// ev_exclusive_block
#define ev_exclusive_block(var) \
    if(bool _ev_exclusive_block_stop = false) {}\
    else for(auto _ev_exclusive_block_lock = var.acquire_exclusive_lock();!_ev_exclusive_block_stop;) \
            for(auto & var = *_ev_exclusive_block_lock.operator->();!_ev_exclusive_block_stop;_ev_exclusive_block_stop=true)




#ifndef exclusive_block
#define exclusive_block ev_exclusive_block
#endif


#define ev_synchronized ev_exclusive_block

#ifndef synchronized
#define synchronized ev_synchronized
#endif


#endif // EV_SHARED_DATA_H
