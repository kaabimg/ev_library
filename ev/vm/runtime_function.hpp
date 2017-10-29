#pragma once

#include <memory>

namespace ev
{
namespace vm
{
namespace jit
{
struct function_private;
}

struct runtime_function_data
{
    runtime_function_data(void* fd);

    uintptr_t function_ptr() const;

protected:
    jit::function_private* m_func_data = nullptr;
};

template <typename T>
struct runtime_function;

template <typename R, typename... Args>
struct runtime_function<R(Args...)> : runtime_function_data
{
    typedef R (*function_type)(Args...);

    runtime_function(void* fd) : runtime_function_data(fd) {}
    R operator()(Args... args) const
    {
        return ((function_type)function_ptr())(args...);
    }
};

template <typename... Args>
struct runtime_function<void(Args...)> : runtime_function_data
{
    typedef void (*function_type)(Args...);

    runtime_function(void* fd) : runtime_function_data(fd) {}
    void operator()(Args... args) const
    {
        return ((function_type)function_ptr())(args...);
    }
};
}
}
