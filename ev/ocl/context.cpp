#include "context.hpp"
#include "device.hpp"

using namespace ev::ocl;

context_t::context_t(const std::vector<device_t> &devices):
    wrapper_type {nullptr}
{
    std::vector<cl_device_id> device_ids;
    device_ids.reserve(devices.size());

    for(device_t device : devices)
        device_ids.push_back(device.cl_data());


    cl_int error_code;
    m_data = clCreateContext(
            nullptr,
            devices.size(),
            device_ids.data(),
            nullptr,
            nullptr,
            &error_code
     );

    check_status(error_code);

}
