#pragma once

#include "memory_object.hpp"
#include "action.hpp"

namespace ev {
namespace ocl {

struct kernel_arg_t {
    kernel_arg_t() {}
    kernel_arg_t(size_t i, cl_mem obj, size_t s)
        : index(i), object(obj), size(s) {}

    size_t index = 0;
    cl_mem object = nullptr;
    size_t size = 0;
};

EV_OCL_DECLARE_CLEAR_FUNCTION(cl_kernel, clRetainKernel, clReleaseKernel)

class kernel_t : public object_wrapper_t<cl_kernel> {
public:
    kernel_t(cl_kernel data) : wrapper_type{data} {}

    template <typename... T>
    exec_kernel_action_t operator()(T... args);

    // properties
    std::string name() const;

private:
    void append_arg(size_t index, buffer_t buffer);

    template <typename H, typename... R>
    void append_arg(size_t index, H h, R... r);

    void flush_args();

    std::string get_string_info(cl_kernel_info info_id) const;
    template <typename T>
    T get_basic_type_info(cl_kernel_info info_id) const;

private:
    std::vector<kernel_arg_t> m_args;
};

template <typename H, typename... R>
void kernel_t::append_arg(size_t index, H h, R... r) {
    static_assert(std::is_same<H, buffer_t>::value,
                  "Expecting a buffer_t argument");

    append_arg(index++, h);
    append_arg(index, r...);
}

template <typename... T>
exec_kernel_action_t kernel_t::operator()(T... args) {
    m_args.resize(sizeof...(T));
    append_arg(0, args...);
    flush_args();

    return exec_kernel_action_t{*this};
}

template <typename T>
T kernel_t::get_basic_type_info(cl_kernel_info info_id) const {
    T info;
    check_status(
        clGetKernelInfo(cl_object(), info_id, sizeof(T), &info, nullptr));
    return info;
}
}
}
