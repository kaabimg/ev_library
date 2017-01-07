#include "program.hpp"
#include "kernel.hpp"
#include "context.hpp"
#include <boost/algorithm/string.hpp>

using namespace ev::ocl;

program_t::program_t(cl_program program) : wrapper_type{program} {}

context_t program_t::contex() const {
    return context_t{get_basic_type_info<cl_context>(CL_PROGRAM_CONTEXT)};
}

void program_t::build(const std::string& options) {
    const char* options_str = nullptr;

    if (!options.empty()) { options_str = options.c_str(); }

    check_status(clBuildProgram(cl_object(), 0, 0,  // devices
                                options_str, on_program_built, this));
}

kernel_t program_t::get_kenel(const std::string& name) const {
    cl_int    status;
    cl_kernel kernel = clCreateKernel(cl_object(), name.c_str(), &status);
    check_status(status);
    return kernel_t{kernel};
}

std::vector<std::string> program_t::kernel_names() const {
    std::string kernels_str = get_string_info(CL_PROGRAM_KERNEL_NAMES);
    std::vector<std::string> names;
    boost::algorithm::split(names, kernels_str,
                            boost::algorithm::is_any_of(";"));
    return names;
}

std::string program_t::source() const {
    return get_string_info(CL_PROGRAM_SOURCE);
}

std::string program_t::build_log() const {
    return get_string_info(CL_PROGRAM_BUILD_LOG);
}

void program_t::on_program_built(cl_program, void*) {}

std::string program_t::get_string_info(cl_program_info info_id) const {
    size_t size;
    check_status(clGetProgramInfo(cl_object(), info_id, 0, nullptr, &size));
    std::string info;
    if (size) {
        char name_str[size];
        check_status(
            clGetProgramInfo(cl_object(), info_id, size, name_str, nullptr));
        info.resize(size - 1);
        std::copy(name_str, name_str + size - 1, info.begin());
    }
    return info;
}
