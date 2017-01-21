#pragma once

#include "preprocessor.hpp"
#include <exception>

namespace ev
{
namespace detail
{
//////
template <typename Func>
struct scope_exit_proactor_t
{
    Func action;
    scope_exit_proactor_t(Func&& f) : action(std::forward<Func>(f)) {}
    ~scope_exit_proactor_t() { action(); }
};

struct scope_exit_builder_t
{
    template <typename Func>
    scope_exit_proactor_t<Func> operator<<(Func&& f)
    {
        return scope_exit_proactor_t<Func>(std::forward<Func>(f));
    }
};

//////

template <typename Cond, typename Func>
struct conditional_scope_exit_proactor_t
{
    Cond cond;
    Func action;
    conditional_scope_exit_proactor_t(Cond&& cond, Func&& f)
        : cond(std::forward<Cond>(cond)), action(std::forward<Func>(f))
    {
    }

    ~conditional_scope_exit_proactor_t()
    {
        if (cond()) action();
    }
};

template <typename Cond>
struct scope_exit_condition_t
{
    Cond cond;

    scope_exit_condition_t(Cond&& c) : cond(std::forward<Cond>(c)) {}
    template <typename Func>
    conditional_scope_exit_proactor_t<Cond, Func> operator<<(Func&& f)
    {
        return conditional_scope_exit_proactor_t<Cond, Func>(
            std::move(cond), std::forward<Func>(f));
    }
};

inline auto make_scope_exit_condition(auto&& c)
{
    typedef typename std::remove_reference<decltype(c)>::type condition_type;
    return scope_exit_condition_t<condition_type>(
        std::forward<condition_type>(c));
}

//////

struct exceptions_counter_t
{
    const int value;

    exceptions_counter_t() : value(std::uncaught_exceptions()) {}
    bool has_new_exceptions() const
    {
        return std::uncaught_exceptions() != value;
    }
};

template <typename Func, bool condition>
struct exception_proactor_t
{
    exceptions_counter_t counter;
    Func action;

    exception_proactor_t(Func&& f) : action(std::forward<Func>(f)) {}
    ~exception_proactor_t()
    {
        if (counter.has_new_exceptions() == condition) {
            action();
        }
    }
};

template <bool condition>
struct exception_proactor_builder_t
{
    template <typename Fun>
    exception_proactor_t<Fun, condition> operator<<(Fun&& f)
    {
        return exception_proactor_t<Fun, condition>(std::forward<Fun>(f));
    }
};
}
}

// ev_on_scope_exit

#define ev_on_scope_exit                            \
    auto EV_ANONYMOUS_VARIABLE(_ev_on_scope_exit) = \
        ::ev::detail::scope_exit_builder_t() << [&]() noexcept

#ifndef on_scope_exit
#define on_scope_exit ev_on_scope_exit
#endif

// ev_on_scope_exit
#define ev_conditional_scope_exit(cond)                          \
    auto EV_ANONYMOUS_VARIABLE(_ev_conditional_scope_exit) =     \
        ::ev::detail::make_scope_exit_condition([&]() noexcept { \
            return (cond);                                       \
        })                                                       \
        << [&]() noexcept

#ifndef conditional_scope_exit
#define conditional_scope_exit ev_conditional_scope_exit
#endif

// ev_on_scope_exit_with_exception
#define ev_on_scope_exit_with_exception          \
    auto EV_ANONYMOUS_VARIABLE(_ev_scope_exit) = \
        ev::detail::exception_proactor_builder_t<true>() << [&]() noexcept

#ifndef on_scope_exit_with_exception
#define on_scope_exit_with_exception ev_on_scope_exit_with_exception
#endif

#define ev_on_scope_exit_without_exception       \
    auto EV_ANONYMOUS_VARIABLE(_ev_scope_exit) = \
        ev::detail::exception_proactor_builder_t<false>() << [&]() noexcept

#ifndef on_scope_exit_without_exception
#define on_scope_exit_without_exception ev_on_scope_exit_without_exception
#endif
