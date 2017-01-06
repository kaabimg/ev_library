#pragma once

#include <memory>

namespace ev {
namespace vm {

namespace jit {
struct function_private_t;
}

struct runtime_function_data_t {
    runtime_function_data_t(void* fd);

    uintptr_t function_ptr() const;

protected:
    jit::function_private_t* m_func_data = nullptr;
};

template <typename T>
struct runtime_function_t;

template <typename R, typename... Args>
struct runtime_function_t<R(Args...)> : runtime_function_data_t {
    typedef R (*function_type)(Args...);

    runtime_function_t(void* fd) : runtime_function_data_t(fd) {}

    R operator()(Args... args) const {
        return ((function_type)function_ptr())(args...);
    }
};

template <typename... Args>
struct runtime_function_t<void(Args...)> : runtime_function_data_t {
    typedef void (*function_type)(Args...);

    runtime_function_t(void* fd) : runtime_function_data_t(fd) {}

    void operator()(Args... args) const {
        return ((function_type)function_ptr())(args...);
    }
};
}
}
