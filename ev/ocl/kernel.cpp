#include "kernel.hpp"

using namespace ev::ocl;

std::string kernel_t::name() const
{
    return get_string_info(CL_KERNEL_FUNCTION_NAME);
}

void kernel_t::append_arg(size_t index, buffer_t buffer)
{
    m_args[index] = kernel_arg_t{index, buffer.cl_object(), buffer.size()};
}

void kernel_t::flush_args()
{
    for (const kernel_arg_t& arg : m_args) {
        check_status(clSetKernelArg(cl_object(), arg.index, sizeof(cl_mem),
                                    &arg.object));
    }
}

std::string kernel_t::get_string_info(cl_kernel_info info_id) const
{
    size_t size;
    check_status(clGetKernelInfo(cl_object(), info_id, 0, nullptr, &size));
    std::string info;
    if (size) {
        char name_str[size];
        check_status(
            clGetKernelInfo(cl_object(), info_id, size, name_str, nullptr));
        info.resize(size - 1);
        std::copy(name_str, name_str + size - 1, info.begin());
    }
    return info;
}
