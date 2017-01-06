#pragma once

#include "common.hpp"
#include "action.hpp"
namespace ev {
namespace ocl {

class context_t;

EV_OCL_DECLARE_CLEAR_FUNCTION(cl_mem, clRetainMemObject, clReleaseMemObject)

class buffer_t : public object_wrapper_t<cl_mem> {
public:
    buffer_t(const context_t& context,
             flags_t<memory_flags_e> flags,
             size_t size);

    template <typename T>
    write_buffer_action_t operator<<(const std::vector<T>& data);

    template <typename T>
    read_buffer_action_t operator>>(std::vector<T>& data);

    // properties

    size_t size() const;

private:
    write_buffer_action_t read_from(void* data, size_t size);
    read_buffer_action_t write_to(void* data);

    template <typename T>
    T get_basic_type_info(cl_mem_info info_id) const;
};

template <typename T>
write_buffer_action_t buffer_t::operator<<(const std::vector<T>& data) {
    return read_from(const_cast<T*>(data.data()), data.size() * sizeof(T));
}

template <typename T>
read_buffer_action_t buffer_t::operator>>(std::vector<T>& data) {
    data.resize(size() / sizeof(T));
    return write_to(data.data());
}

template <typename T>
T buffer_t::get_basic_type_info(cl_device_info info_id) const {
    T info;
    check_status(
        clGetMemObjectInfo(cl_object(), info_id, sizeof(T), &info, nullptr));
    return info;
}
}
}
