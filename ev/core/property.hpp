#pragma once

#include "basic_types.hpp"

#include <functional>
#include <vector>
#include <type_traits>

namespace ev {

template <typename T>
struct property_trait {
    static constexpr std::size_t max_size = sizeof(uint64_t) * 2;
    static constexpr bool is_trivial = std::is_trivial<T>::value && sizeof(T) <= max_size;

    using type = T;
    using access_type = typename std::conditional<is_trivial, T, const T&>::type;
};

template <class P>
struct property_notifier {
    using ObserverSignature = void(P&);
    using Observer = std::function<ObserverSignature>;

    inline ~property_notifier()
    {
        if (_observers) {
            delete _observers;
        }
    }

    inline constexpr size_t add_observer(auto&& observer)
    {
        if (!_observers) _observers = new std::vector<Observer>(1);
        _observers->emplace_back(std::forward<decltype(observer)>(observer));
        return _observers->size() - 1;
    }
    inline constexpr void remove_observer(size_t index)
    {
        if (_observers) {
            (*_observers)[index] = {};
        }
    }

protected:
    inline constexpr void on_changed(P& p)
    {
        if (_observers) {
            for (auto obs : *_observers) {
                if (obs) {
                    obs(p);
                }
            }
        }
    }
    std::vector<Observer>* _observers = nullptr;
};

template <typename P>
struct empty_property_notifier {
protected:
    inline constexpr void on_changed(P&)
    {
    }
};

template <typename P>
struct single_property_notifier {
    using ObserverSignature = void(P&);
    using Observer = std::function<ObserverSignature>;

    inline void set_observer(auto&& obs)
    {
        _observer = std::forward<decltype(obs)>(obs);
    }

    inline void clear_observer()
    {
        _observer = {};
    }

protected:
    inline constexpr void on_changed(P& p)
    {
        if (_observer) _observer(p);
    }

private:
    Observer _observer;
};

#define __EV_PROPERTY_FORWARD_OPERATOR(op)    \
    inline bool operator op(auto&& val) const \
    {                                         \
        if                                    \
            constexpr(is_property_value(val)) \
            {                                 \
                return _value op val._value;  \
            }                                 \
        else {                                \
            return _value op val;             \
        }                                     \
    }
template <typename T>
struct property_value {
    static constexpr bool is_property_value(auto&& val)
    {
        return std::is_base_of<property_value<T>, typename std::decay<decltype(val)>::type>::value;
    }
    // clang-format off

    inline constexpr property_value()
    {
        if constexpr(std::is_pointer<T>::value)
        {
            _value = nullptr;
        }
    }

    inline constexpr property_value(auto&& val)
    {
        if constexpr(is_property_value(val))
        {
            if (std::is_rvalue_reference<decltype(val)>::value)
            {
                _value =std::move(val._value);
            }
            else
            {
                _value = val._value;
            }
        }
        else {
            _value = std::forward<decltype(val)>(val);
        }
    }

    inline ~property_value()
    {
        if constexpr(std::is_pointer<T>::value)
        {
            delete _value;
        }
    }


    T* operator ->()
    {
        return &_value;
    }
    const T* operator ->() const
    {
        return &_value;
    }


    __EV_PROPERTY_FORWARD_OPERATOR(==)
    __EV_PROPERTY_FORWARD_OPERATOR(!=)
    __EV_PROPERTY_FORWARD_OPERATOR(<=)
    __EV_PROPERTY_FORWARD_OPERATOR(<)
    __EV_PROPERTY_FORWARD_OPERATOR(>=)
    __EV_PROPERTY_FORWARD_OPERATOR(>)

    // clang-format on

protected:
    T _value;
};

template <typename T,
          template <typename P> class notifier = property_notifier,
          typename traits = property_trait<T>>
struct property : property_value<T>, notifier<property<T, notifier, traits>> {
    using notifier_type = notifier<property<T, notifier, traits>>;

    inline constexpr property() : property_value<T>()
    {
    }
    inline constexpr property(auto&& val) : property_value<T>(std::forward<decltype(val)>(val))
    {
    }

    constexpr property(const property&) = delete;

    inline constexpr typename traits::access_type read() const
    {
        return property_value<T>::_value;
    }

    inline constexpr void write(auto&& val)
    {
        property_value<T>::_value = std::forward<decltype(val)>(val);
        notify_changed();
    }

    inline constexpr property& operator=(auto&& val)
    {
        write(std::forward<decltype(val)>(val));
        return *this;
    }

    inline constexpr operator typename traits::access_type() const
    {
        return property_value<T>::_value;
    }

    void notify_changed()
    {
        notifier_type::on_changed(*this);
    }

    using property_value<T>::operator==;
    using property_value<T>::operator!=;
    using property_value<T>::operator<=;
    using property_value<T>::operator<;
    using property_value<T>::operator>=;
    using property_value<T>::operator>;
    using property_value<T>::operator->;
};

template <typename P>
struct scoped_observer : non_copyable {
    inline constexpr scoped_observer(P& property, auto&& action) : _property(property)
    {
        _index = property.add_observer(std::forward<decltype(action)>(action));
    }
    inline ~scoped_observer()
    {
        _property.remove_observer(_index);
    }

private:
    P& _property;
    size_t _index;
};

inline constexpr auto make_scoped_observer(auto&& p, auto&& action)
{
    return scoped_observer<decltype(p)>(p, action);
}
}
