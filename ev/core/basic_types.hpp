#pragma once
#include <utility>

namespace ev {

struct empty_t {
};

struct non_copyable {
    non_copyable() = default;
    ~non_copyable() = default;
    non_copyable(const non_copyable&) = delete;
    non_copyable& operator=(const non_copyable&) = delete;
};

template <typename T = empty_t, bool cond = true>
struct loop_scoped_variable {
    template <typename... A>
    loop_scoped_variable(A&&... d) : data(std::forward<A>(d)...)
    {
    }
    operator bool() const
    {
        return cond;
    }
    T data;
};
}
