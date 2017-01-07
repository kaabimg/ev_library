#pragma once

#include "common.hpp"

namespace ev {
namespace ocl {

EV_OCL_DECLARE_CLEAR_FUNCTION(cl_device_id, clRetainDevice, clReleaseDevice)

class device_t : public object_wrapper_t<cl_device_id> {
public:
    device_t(cl_device_id id, init_mode_e mode = init_mode_e::create)
        : wrapper_type{id, mode} {}

    // properties
    bool          is_available() const;
    std::string   name() const;
    std::string   vendor() const;
    std::string   version() const;
    std::string   driver_version() const;
    std::string   profile() const;
    device_type_e type() const;
    size_t        max_clock_frequency() const;
    size_t        max_compute_units() const;

protected:
    std::string get_string_info(cl_device_info info_id) const;
    template <typename T>
    T get_basic_type_info(cl_device_info info_id) const;
};

template <typename T>
T device_t::get_basic_type_info(cl_device_info info_id) const {
    T info;
    check_status(
        clGetDeviceInfo(cl_object(), info_id, sizeof(T), &info, nullptr));
    return info;
}
}
}
