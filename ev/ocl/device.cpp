#include "device.hpp"

using namespace ev::ocl;

bool device_t::is_available() const {
    return get_basic_type_info<cl_bool>(CL_DEVICE_AVAILABLE);
}

std::string device_t::name() const {
    return get_string_info(CL_DEVICE_NAME);
}

std::string device_t::vendor() const {
    return get_string_info(CL_DEVICE_VENDOR);
}

std::string device_t::version() const {
    return get_string_info(CL_DEVICE_VERSION);
}

std::string device_t::driver_version() const {
    return get_string_info(CL_DRIVER_VERSION);
}

std::string device_t::profile() const {
    return get_string_info(CL_DEVICE_PROFILE);
}

device_type_e device_t::type() const {
    return static_cast<device_type_e>(
        get_basic_type_info<cl_device_type>(CL_DEVICE_TYPE));
}

size_t device_t::max_clock_frequency() const {
    return get_basic_type_info<cl_uint>(CL_DEVICE_MAX_CLOCK_FREQUENCY);
}

size_t device_t::max_compute_units() const {
    return get_basic_type_info<cl_uint>(CL_DEVICE_MAX_COMPUTE_UNITS);
}

///

std::string device_t::get_string_info(cl_device_info info_id) const {
    size_t size;
    check_status(clGetDeviceInfo(cl_object(), info_id, 0, nullptr, &size));
    std::string info;
    if (size) {
        char name_str[size];
        check_status(
            clGetDeviceInfo(cl_object(), info_id, size, name_str, nullptr));
        info.resize(size - 1);
        std::copy(name_str, name_str + size - 1, info.begin());
    }
    return info;
}
