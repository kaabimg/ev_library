#include "platform.hpp"
#include "device.hpp"

#include <ev/core/logging.hpp>
using namespace ev::ocl;

std::vector<ev::ocl::platform_t> ev::ocl::platform_t::get_platforms()
{
    cl_uint count = 0;
    clGetPlatformIDs(0, NULL, &count);
    std::vector<cl_platform_id> ids{count};
    std::vector<platform_t> platforms;
    platforms.reserve(count);
    clGetPlatformIDs(count, ids.data(), nullptr);
    for (auto id : ids) platforms.emplace_back(id);
    return platforms;
}

std::vector<device_t> platform_t::get_devices(flags_t<device_type_e> type) const
{
    std::vector<device_t> devices;
    std::vector<cl_device_id> device_ids;
    cl_uint count = 0;
    clGetDeviceIDs(cl_object(), type.data(), 0, nullptr, &count);
    device_ids.resize(count);
    clGetDeviceIDs(cl_object(), type.data(), count, device_ids.data(), nullptr);
    for (auto id : device_ids) devices.emplace_back(id);
    return devices;
}

std::string platform_t::name() const
{
    return get_string_info(CL_PLATFORM_NAME);
}

std::string platform_t::profile() const
{
    return get_string_info(CL_PLATFORM_PROFILE);
}

std::string platform_t::version() const
{
    return get_string_info(CL_PLATFORM_VERSION);
}

std::string platform_t::vendor() const
{
    return get_string_info(CL_PLATFORM_VENDOR);
}

std::string platform_t::extensions() const
{
    return get_string_info(CL_PLATFORM_EXTENSIONS);
}

std::string platform_t::get_string_info(cl_platform_info info_id) const
{
    size_t size;
    check_status(clGetPlatformInfo(cl_object(), info_id, 0, nullptr, &size));
    std::string info;
    if (size) {
        char name_str[size];
        check_status(
            clGetPlatformInfo(cl_object(), info_id, size, name_str, nullptr));
        info.resize(size - 1);
        std::copy(name_str, name_str + size - 1, info.begin());
    }
    return info;
}

std::vector<platform_t> get_platfomrs()
{
    return platform_t::get_platforms();
}
