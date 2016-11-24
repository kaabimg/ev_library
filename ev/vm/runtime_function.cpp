#include "runtime_function.h"
#include "jit/private_data.h"

using namespace ev::vm;
runtime_function_data_t::runtime_function_data_t(void *fd):
    m_func_data((jit::function_private_t*)fd)
{}

uintptr_t ev::vm::runtime_function_data_t::function_ptr() const
{
    return m_func_data->function_ptr;
}
