#pragma once

#include "algorithm.hpp"
#include "traits.hpp"
#include <functional>
#include <vector>
#include <type_traits>

// clang-format off
namespace ev {
template <typename P>
using property_observer_signature = void(const P&);

template <typename P>
using property_argless_observer_signature = void();

template <typename P>
using observer_type = std::function<property_observer_signature<P>>;

template <typename P>
using argless_observer_type = std::function<property_argless_observer_signature<P>>;


template <typename P>
inline constexpr observer_type<P> make_observer(observer_type<P> o)
{
    return o;
}


template <typename P>
inline constexpr observer_type<P> make_observer(argless_observer_type<P> o)
{
    return [obs = std::move(o)](const P&){obs();};
}


template <typename T>
struct property_trait {
    static constexpr std::size_t max_size = sizeof(uint64_t) * 2;
    static constexpr bool is_trivial = std::is_trivial<T>::value && sizeof(T) <= max_size;

    using type = T;
    using access_type = typename std::conditional<is_trivial, T, const T&>::type;
};

template <class P>
struct property_notifier {
    using observer = observer_type<P>;

    property_notifier()=default;
    inline constexpr property_notifier(const property_notifier& another){
        if(another._observers)
            _observers = new std::vector<observer>(*another._observers);
    }
    inline constexpr property_notifier(property_notifier&& another){
        std::swap(_observers,another._observers);
    }
    inline constexpr property_notifier& operator=(const property_notifier& another){
        if(another._observers)
        {
            if(_observers) delete _observers;
            _observers = new std::vector<observer>(*another._observers);
        }
        return *this;
    }

    inline constexpr property_notifier& operator=(property_notifier&& another){
        std::swap(_observers,another._observers);
        return *this;
    }

    inline ~property_notifier()
    {
        if (_observers) {
            delete _observers;
        }
    }

    inline constexpr size_t add_observer(auto&& obs)
    {
        if (!_observers) _observers = new std::vector<observer>(1);
        _observers->emplace_back(make_observer<P>(std::forward<decltype(obs)>(obs)));
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
    std::vector<observer>* _observers = nullptr;
};

template <typename P>
struct no_property_notifier {
protected:
    inline constexpr void on_changed(P&)
    {
    }
};

template <typename P>
struct single_property_notifier {
    using observer = observer_type<P>;

    single_property_notifier() = default;
    single_property_notifier(const single_property_notifier&) = default;
    single_property_notifier(single_property_notifier&&) = default;

    single_property_notifier& operator=(const single_property_notifier&) = default;
    single_property_notifier& operator=(single_property_notifier&&) = default;


    inline constexpr void set_observer(auto&& obs)
    {
        _observer = make_observer<P>(std::forward<decltype(obs)>(obs));
    }

    inline constexpr void clear_observer()
    {
        _observer = {};
    }

protected:
    inline constexpr void on_changed(P& p)
    {
        if (_observer) _observer(p);
    }

private:
    observer _observer;
};

#define EV_PROPERTY_FORWARD_OPERATOR(op)                \
    inline constexpr bool operator op(auto&& val) const \
    {                                                   \
        if constexpr(is_property_value(val))            \
        {                                               \
            return _value op val._value;                \
        }                                               \
        else                                            \
        {                                               \
            return _value op val;                       \
        }                                               \
    }

template <typename T>
struct property_value {
    static constexpr bool is_property_value(auto&& val)
    {
        return std::is_base_of<property_value<T>, typename std::decay<decltype(val)>::type>::value;
    }

    property_value(const property_value&) = default;
    property_value(property_value&&) = default;
    property_value& operator=(const property_value&) = default;
    property_value& operator=(property_value&&) = default;

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
            if (std::is_rvalue_reference<decltype(val)>::value) {
                _value = std::move(val._value);
            }
            else {
                _value = val._value;
            }
        }
        else
        {
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

    inline constexpr T* operator->()
    {
        return &_value;
    }
    inline constexpr const T* operator->() const
    {
        return &_value;
    }

    inline constexpr T& operator*()
    {
        return _value;
    }
    inline constexpr const T& operator*() const
    {
        return _value;
    }

    EV_PROPERTY_FORWARD_OPERATOR(==)
    EV_PROPERTY_FORWARD_OPERATOR(!=)
    EV_PROPERTY_FORWARD_OPERATOR(<=)
    EV_PROPERTY_FORWARD_OPERATOR(<)
    EV_PROPERTY_FORWARD_OPERATOR(>=)
    EV_PROPERTY_FORWARD_OPERATOR(>)

protected:
    T _value;
};

template <typename T,
          template <typename P> class notifier = property_notifier,
          typename traits = property_trait<T>>
struct property : property_value<T>, notifier<property<T, notifier, traits>> {
    using notifier_type = notifier<property<T, notifier, traits>>;
    using type = T;
    using access_type = typename traits::access_type;
    using value_holder_type = property_value<T>;

    inline constexpr property() : value_holder_type()
    {
    }
    inline constexpr property(auto&& val) : value_holder_type(std::forward<decltype(val)>(val))
    {
    }

    property(const property&) = default;
    property& operator=(const property&) = default;
    property(property&&) = default;
    property& operator=(property&&) = default;

    inline constexpr access_type read() const
    {
        return value_holder_type::_value;
    }

    inline constexpr void write(auto&& val)
    {
        value_holder_type::_value = std::forward<decltype(val)>(val);
        notify_changed();
    }

    inline constexpr property& operator=(auto&& val)
    {
        write(std::forward<decltype(val)>(val));
        return *this;
    }

    inline constexpr operator access_type() const
    {
        return value_holder_type::_value;
    }

    inline void notify_changed()
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
    using property_value<T>::operator*;
};

template <typename P>
struct scoped_observer {
    inline constexpr scoped_observer(P& property, auto&& action) : _property(property)
    {
        _index = property.add_observer(std::forward<decltype(action)>(action));
    }

    scoped_observer(const scoped_observer&) = delete;
    scoped_observer(scoped_observer&&) = delete;

    scoped_observer& operator=(const scoped_observer&) = delete;
    scoped_observer& operator=(scoped_observer&&) = delete;

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

template <typename... Fs>
struct observer_pack {
    constexpr observer_pack(Fs&&... fs) : _observers{std::forward<Fs>(fs)...}
    {
    }

    template <typename P>
    constexpr void operator()(const P& p)
    {
        ev::for_each(_observers,[&p](auto& observer){
            observer(p);
        });
    }

private:
    std::tuple<Fs...> _observers;
};

template <typename... Fs>
inline constexpr observer_pack<Fs...> make_observer_pack(Fs&&... fs)
{
    return observer_pack<Fs...>(std::forward<Fs>(fs)...);
}
}
// clang-format on
