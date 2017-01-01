#pragma once


#include <shared_mutex>

#include "preprocessor.hpp"

namespace ev {

template <typename Mutex>
struct lock_helper_t {

    static inline void lock(Mutex& m ){m.lock();}
    static inline void unlock(Mutex& m ){m.unlock();}

    static inline void lock_shared(Mutex& m ){m.lock_shared();}
    static inline void unlock_shared(Mutex& m ){m.unlock_shared();}

};



template <>
struct lock_helper_t<std::mutex> {

    static inline void lock(std::mutex& m ){m.lock();}
    static inline void unlock(std::mutex& m ){m.unlock();}

    static inline void lock_shared(std::mutex& m ){m.lock();}
    static inline void unlock_shared(std::mutex& m ){m.unlock();}

};



template <typename T,typename Mutex = std::shared_mutex>
class synchronized_data_t {
    T m_data;
    mutable Mutex m_mutex;

public:

    using data_type = T;
    using mutex_type = Mutex;

    template <typename TA,
              void (lock_function)(Mutex&),
              void (unlock_function)(Mutex&)>
    class lock_t
    {
        synchronized_data_t* m_data = nullptr;

    public:
        inline lock_t(synchronized_data_t * d):m_data(d){
            lock();
        }

        inline lock_t(const lock_t & rhs):lock_t(rhs.m_data){}

        inline lock_t(lock_t && rhs){
            std::swap(m_data,rhs.m_data);
        }

        inline ~lock_t(){
            unlock();
        }


        inline lock_t& operator =(const lock_t & rhs){
            unlock();
            m_data = rhs.m_data;
            lock();
            return *this;
        }

        inline lock_t& operator =(lock_t && rhs){
            std::swap(m_data,rhs.m_data);
            return *this;
        }

        inline TA* operator->(){
            return &m_data->m_data;
        }

    private:
        void lock(){
            if(m_data) lock_function(m_data->m_mutex);
        }
        void unlock(){
            if(m_data) unlock_function(m_data->m_mutex);
        }
    };



    using shared_lock_t = lock_t<
        const T,
        lock_helper_t<Mutex>::lock_shared,
        lock_helper_t<Mutex>::unlock_shared
        >;
    using exclusive_lock_t = lock_t<
        T,
        lock_helper_t<Mutex>::lock,
        lock_helper_t<Mutex>::unlock
        >;

    /// synchronized_data_t


    synchronized_data_t()noexcept(std::is_nothrow_constructible<T>::value){}

    synchronized_data_t(const synchronized_data_t & rhs)noexcept(std::is_nothrow_copy_assignable<T>::value)
    {
        auto read_lock = rhs.acquire_shared_lock();
        ev_unused(read_lock);
        m_data = rhs.m_data;
    }

    synchronized_data_t(synchronized_data_t && rhs)noexcept(std::is_nothrow_move_constructible<T>::value)
        :m_data(std::move(rhs.m_data)){}


    synchronized_data_t(const T& d)noexcept(std::is_nothrow_copy_constructible<T>::value)
        :m_data(d){}
    synchronized_data_t(T&& d):m_data(std::move(d)){}



    synchronized_data_t & operator = (const synchronized_data_t & rhs)noexcept(std::is_nothrow_copy_assignable<T>::value)
    {

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


    synchronized_data_t & operator = (synchronized_data_t && rhs)noexcept(std::is_nothrow_move_assignable<T>::value)
    {
        auto lock = acquire_exclusive_lock();
        ev_unused(lock);
        m_data = std::move(rhs);
        return *this;
    }


    inline const shared_lock_t acquire_shared_lock()const{
        return shared_lock_t(const_cast<synchronized_data_t*>(this));
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

    inline const synchronized_data_t& as_const()const{
        return *this;
    }

    inline T copy()const{
        auto lock = acquire_shared_lock();
        ev_unused(lock);
        return m_data;
    }
};



} //ev


#define ev_synchronized(...) EV_CONCAT(__ev_synchronized_,EV_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)
#define __ev_synchronized_1(var) __ev_synchronized_2(var,var)
#define __ev_synchronized_2(var,expression) \
    if(bool __ev_synchronized_2_stop = false) {}\
    else for(auto _ev_sync_lock = (expression).operator->();!__ev_synchronized_2_stop;) \
    for(auto & var = *_ev_sync_lock.operator->();!__ev_synchronized_2_stop;__ev_synchronized_2_stop=true)

#ifndef synchronized
#define synchronized ev_synchronized
#endif

