#pragma once

#include <ev/core/preprocessor.hpp>
#include <shared_mutex>

namespace ev {
template <typename Mutex>
struct lock_helper {
    static inline void lock(Mutex& m)
    {
        m.lock();
    }
    static inline void unlock(Mutex& m)
    {
        m.unlock();
    }
    static inline void lock_shared(Mutex& m)
    {
        m.lock_shared();
    }
    static inline void unlock_shared(Mutex& m)
    {
        m.unlock_shared();
    }
};

template <>
struct lock_helper<std::mutex> {
    static inline void lock(std::mutex& m)
    {
        m.lock();
    }
    static inline void unlock(std::mutex& m)
    {
        m.unlock();
    }
    static inline void lock_shared(std::mutex& m)
    {
        m.lock();
    }
    static inline void unlock_shared(std::mutex& m)
    {
        m.unlock();
    }
};

template <typename T, typename Mutex = std::shared_mutex>
class synchronized_data {
    T _data;
    mutable Mutex _mutex;

public:
    using data_type = T;
    using mutex_type = Mutex;

    template <typename TA, void(lock_function)(Mutex&), void(unlock_function)(Mutex&)>
    class locker {
        synchronized_data* _data = nullptr;

    public:
        inline locker(synchronized_data* d) : _data(d)
        {
            lock();
        }
        inline locker(const locker& rhs) : locker(rhs._data)
        {
        }
        inline locker(locker&& rhs)
        {
            std::swap(_data, rhs._data);
        }
        inline ~locker()
        {
            unlock();
        }
        inline locker& operator=(const locker& rhs)
        {
            unlock();
            _data = rhs._data;
            lock();
            return *this;
        }

        inline locker& operator=(locker&& rhs)
        {
            std::swap(_data, rhs._data);
            return *this;
        }

        inline TA* operator->()
        {
            return &_data->_data;
        }

    private:
        void lock()
        {
            if (_data) lock_function(_data->_mutex);
        }
        void unlock()
        {
            if (_data) unlock_function(_data->_mutex);
        }
    };

    using shared_lock =
        locker<const T, lock_helper<Mutex>::lock_shared, lock_helper<Mutex>::unlock_shared>;
    using exclusive_lock = locker<T, lock_helper<Mutex>::lock, lock_helper<Mutex>::unlock>;

    /// synchronized_data_t

    synchronized_data() noexcept(std::is_nothrow_constructible<T>::value)
    {
    }
    synchronized_data(const synchronized_data& rhs) noexcept(
        std::is_nothrow_copy_assignable<T>::value)
    {
        auto read_lock = rhs.acquire_shared_lock();
        ev_unused(read_lock);
        _data = rhs._data;
    }

    synchronized_data(synchronized_data&& rhs) noexcept(
        std::is_nothrow_move_constructible<T>::value)
        : _data(std::move(rhs._data))
    {
    }

    synchronized_data(const T& d) noexcept(std::is_nothrow_copy_constructible<T>::value) : _data(d)
    {
    }
    synchronized_data(T&& d) : _data(std::move(d))
    {
    }
    synchronized_data& operator=(const synchronized_data& rhs) noexcept(
        std::is_nothrow_copy_assignable<T>::value)
    {
        if (this < &rhs) {
            auto wl = acquire_exclusive_lock();
            auto rl = rhs.acquire_shared_lock();
            ev_unused(wl);
            ev_unused(rl);
            _data = rhs._data;
        }
        else if (this > &rhs) {
            auto rl = rhs.acquire_shared_lock();
            auto wl = acquire_exclusive_lock();
            ev_unused(wl);
            ev_unused(rl);
            _data = rhs._data;
        }
        return *this;
    }

    synchronized_data& operator=(synchronized_data&& rhs) noexcept(
        std::is_nothrow_move_assignable<T>::value)
    {
        auto lock = acquire_exclusive_lock();
        ev_unused(lock);
        _data = std::move(rhs);
        return *this;
    }

    inline const shared_lock acquire_shared_lock() const
    {
        return shared_lock(const_cast<synchronized_data*>(this));
    }

    inline exclusive_lock acquire_exclusive_lock()
    {
        return exclusive_lock(this);
    }

    inline shared_lock operator->() const
    {
        return acquire_shared_lock();
    }
    inline exclusive_lock operator->()
    {
        return acquire_exclusive_lock();
    }
    inline const synchronized_data& as_const() const
    {
        return *this;
    }
    inline T copy() const
    {
        auto lock = acquire_shared_lock();
        ev_unused(lock);
        return _data;
    }
};

}  // ev

#define ev_synchronized(...)                                 \
    EV_CONCAT(__ev_synchronized_, EV_ARG_COUNT(__VA_ARGS__)) \
    (__VA_ARGS__)
#define __ev_synchronized_1(var) __ev_synchronized_2(var, var)
#define __ev_synchronized_2(var, expression)                  \
    if (auto _ev_sync_lock = (expression).operator->(); true) \
        if (auto& var = *_ev_sync_lock.operator->(); true)

#ifndef synchronized
#define synchronized ev_synchronized
#endif
