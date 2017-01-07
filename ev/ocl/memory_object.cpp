#include "memory_object.hpp"
#include "context.hpp"

using namespace ev::ocl;

#include <bitset>

buffer_t::buffer_t(const context_t&        context,
                   flags_t<memory_flags_e> flags,
                   size_t                  size)
    : wrapper_type{nullptr} {
    cl_int status;
    set_data(clCreateBuffer(context.cl_object(), flags.data(), size, nullptr,
                            &status),
             init_mode_e::create);

    check_status(status);
}

size_t buffer_t::size() const {
    return get_basic_type_info<size_t>(CL_MEM_SIZE);
}

write_buffer_action_t buffer_t::read_from(void* data, size_t size) {
    return write_buffer_action_t{*this, data, size};
}

read_buffer_action_t buffer_t::write_to(void* data) {
    return read_buffer_action_t(*this, data, size());
}
