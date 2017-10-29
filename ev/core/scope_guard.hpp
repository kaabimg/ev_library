#pragma once

#include "preprocessor.hpp"
#include <exception>

namespace ev {
namespace detail {
//////
template <typename Func>
struct scope_exit_proactor {
    Func action;
    scope_exit_proactor(Func&& f) : action(std::forward<Func>(f))
    {
    }
    ~scope_exit_proactor()
    {
        action();
    }
};

struct scope_exit_builder {
    template <typename Func>
    scope_exit_proactor<Func> operator<<(Func&& f)
    {
        return scope_exit_proactor<Func>(std::forward<Func>(f));
    }
};

//////

template <typename Cond, typename Func>
struct conditional_scope_exit_proactor {
    Cond cond;
    Func action;
    conditional_scope_exit_proactor(Cond&& cond, Func&& f)
        : cond(std::forward<Cond>(cond)), action(std::forward<Func>(f))
    {
    }

    ~conditional_scope_exit_proactor()
    {
        if (cond()) action();
    }
};

template <typename Cond>
struct scope_exit_condition {
    Cond cond;

    scope_exit_condition(Cond&& c) : cond(std::forward<Cond>(c))
    {
    }
    template <typename Func>
    conditional_scope_exit_proactor<Cond, Func> operator<<(Func&& f)
    {
        return conditional_scope_exit_proactor<Cond, Func>(std::move(cond), std::forward<Func>(f));
    }
};

inline auto make_scope_exit_condition(auto&& c)
{
    typedef typename std::remove_reference<decltype(c)>::type condition_type;
    return scope_exit_condition<condition_type>(std::forward<condition_type>(c));
}

//////

struct exceptions_counter {
    const int value;

    exceptions_counter() : value(std::uncaught_exceptions())
    {
    }
    bool has_new_exceptions() const
    {
        return std::uncaught_exceptions() != value;
    }
};

template <typename Func, bool condition>
struct exception_proactor {
    exceptions_counter counter;
    Func action;

    exception_proactor(Func&& f) : action(std::forward<Func>(f))
    {
    }
    ~exception_proactor()
    {
        if (counter.has_new_exceptions() == condition) {
            action();
        }
    }
};

template <bool condition>
struct exception_proactor_builder {
    template <typename Fun>
    exception_proactor<Fun, condition> operator<<(Fun&& f)
    {
        return exception_proactor<Fun, condition>(std::forward<Fun>(f));
    }
};
}
}

// ev_on_scope_exit

#define ev_on_scope_exit                                                               \
    auto EV_ANONYMOUS_VARIABLE(_ev_on_scope_exit) = ::ev::detail::scope_exit_builder() \
                                                    << [&]() noexcept

#ifndef on_scope_exit
#define on_scope_exit ev_on_scope_exit
#endif

// ev_on_scope_exit
#define ev_conditional_scope_exit(cond)                                            \
    auto EV_ANONYMOUS_VARIABLE(_ev_conditional_scope_exit) =                       \
        ::ev::detail::make_scope_exit_condition([&]() noexcept { return (cond); }) \
        << [&]() noexcept

#ifndef conditional_scope_exit
#define conditional_scope_exit ev_conditional_scope_exit
#endif

// ev_on_scope_exit_with_exception
#define ev_on_scope_failure                                                                     \
    auto EV_ANONYMOUS_VARIABLE(_ev_scope_exit) = ev::detail::exception_proactor_builder<true>() \
                                                 << [&]() noexcept

#ifndef on_scope_failure
#define on_scope_failure ev_on_scope_failure
#endif

#define ev_on_scope_success                                                                      \
    auto EV_ANONYMOUS_VARIABLE(_ev_scope_exit) = ev::detail::exception_proactor_builder<false>() \
                                                 << [&]() noexcept

#ifndef on_scope_success
#define on_scope_success ev_on_scope_success
#endif

#ifndef on_scope
#define on_scope(how) on_scope_##how
#endif
