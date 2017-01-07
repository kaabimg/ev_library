#include "context.hpp"
#include "command_queue.hpp"
#include "device.hpp"
#include "memory_object.hpp"
#include "program.hpp"

#include <fstream>

using namespace ev::ocl;

context_t::context_t(const std::vector<device_t>& devices)
    : wrapper_type{nullptr} {
    std::vector<cl_device_id> device_ids;
    device_ids.reserve(devices.size());

    for (device_t device : devices) device_ids.push_back(device.cl_object());

    cl_int status;
    set_data(clCreateContext(nullptr, devices.size(), device_ids.data(),
                             nullptr, nullptr, &status),
             init_mode_e::create);

    check_status(status);
}

context_t::context_t(cl_context c) : wrapper_type{c} {}

std::vector<device_t> context_t::devices() const {
    std::vector<device_t> devices;

    size_t size;

    check_status(
        clGetContextInfo(cl_object(), CL_CONTEXT_DEVICES, 0, nullptr, &size));

    cl_device_id ids[size / sizeof(cl_device_id)];

    check_status(
        clGetContextInfo(cl_object(), CL_CONTEXT_DEVICES, size, ids, nullptr));

    devices.reserve(size / sizeof(cl_device_id));

    for (int i = 0; i < size / sizeof(cl_device_id); ++i) {
        devices.emplace_back(ids[i], init_mode_e::reuse);
    }

    return devices;
}

program_t context_t::new_program_from_file(
    const std::string&              path,
    const std::vector<std::string>& include_dirs) {
    std::ifstream input_file{path};
    if (input_file) {
        return new_program_from_sources(
            std::string{std::istreambuf_iterator<char>(input_file),
                        std::istreambuf_iterator<char>()},
            include_dirs);
    }

    throw std::runtime_error("Failed to read " + path);
}

program_t context_t::new_program_from_sources(
    const std::string&              program_txt,
    const std::vector<std::string>& include_dirs)

{
    cl_int      status;
    const char* str     = program_txt.c_str();
    cl_program  program = clCreateProgramWithSource(
        cl_object(), 1, (const char**)&str, nullptr, &status);

    check_status(status);

    return program_t{program};
}

command_queue_t context_t::new_command_queue(device_t device) {
    return command_queue_t{*this, device};
}

buffer_t context_t::create_buffer(size_t size, flags_t<memory_flags_e> flags) {
    return buffer_t{*this, flags, size};
}
