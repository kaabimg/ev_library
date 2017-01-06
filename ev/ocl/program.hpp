#pragma once

#include "common.hpp"

namespace ev {
namespace ocl {

class kernel_t;
class context_t;

EV_OCL_DECLARE_CLEAR_FUNCTION(cl_program, clRetainProgram, clReleaseProgram)

class program_t : public object_wrapper_t<cl_program> {
public:
    program_t(cl_program program);

    context_t contex() const;

    void build(const std::string& options = std::string());

    kernel_t get_kenel(const std::string& name) const;

    // properties
    std::vector<std::string> kernel_names() const;
    std::string source() const;
    std::string build_log() const;

private:
    static void on_program_built(cl_program program, void* user_data);

    std::string get_string_info(cl_program_info info_id) const;
    template <typename T>
    T get_basic_type_info(cl_device_info info_id) const;
};

template <typename T>
T program_t::get_basic_type_info(cl_device_info info_id) const {
    T info;
    check_status(
        clGetProgramInfo(cl_object(), info_id, sizeof(T), &info, nullptr));
    return info;
}
}
}
