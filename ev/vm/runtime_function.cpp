#include "runtime_function.hpp"
#include "jit/private_data.hpp"

using namespace ev::vm;

runtime_function_data::runtime_function_data(void* fd)
    : m_func_data((jit::function_private*)fd)
{
}

uintptr_t ev::vm::runtime_function_data::function_ptr() const
{
    return m_func_data->function_ptr;
}
