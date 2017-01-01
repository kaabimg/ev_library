#pragma once

#include "common.hpp"
#include "memory_object.hpp"
namespace ev { namespace ocl {

class device_t;
class program_t;
class command_queue_t;

EV_OCL_DECLARE_CLEAR_FUNCTION(cl_context,clRetainContext,clReleaseContext)

class context_t : public object_wrapper_t<cl_context>{
public:
    context_t(const std::vector<device_t>& devices);
    context_t(cl_context );

    std::vector<device_t> devices()const;

    template <typename T>
    buffer_t new_buffer(size_t count,
                        flags_t<memory_flags_e> flags = memory_flags_e::read_write);


    program_t new_program_from_file(
            const std::string & path,
            const std::vector<std::string>& include_dirs = std::vector<std::string>());

    program_t new_program_from_sources(
            const std::string & program_txt,
            const std::vector<std::string>& include_dirs = std::vector<std::string>());

    command_queue_t new_command_queue(device_t device);

private:    

    buffer_t create_buffer(size_t size, flags_t<memory_flags_e> flags);
};


template <typename T>
buffer_t context_t::new_buffer(size_t count, flags_t<memory_flags_e> flags )
{
    return create_buffer(count * sizeof(T),flags);
}

}}
