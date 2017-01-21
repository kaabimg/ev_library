#include "common.hpp"

#include <boost/algorithm/string.hpp>

using namespace ev::ocl;

cl_error_t::cl_error_t(cl_int error) : m_error(error)
{
    build_error_string();
}

const char* cl_error_t::what() const noexcept
{
    return m_error_string.c_str();
}

cl_int cl_error_t::error() const
{
    return m_error;
}

#define __CASE_ERROR_CODE(code) \
    case code: m_error_string = #code; break
void cl_error_t::build_error_string()
{
    switch (m_error)
    {
        __CASE_ERROR_CODE(CL_DEVICE_NOT_FOUND);
        __CASE_ERROR_CODE(CL_DEVICE_NOT_AVAILABLE);
        __CASE_ERROR_CODE(CL_COMPILER_NOT_AVAILABLE);
        __CASE_ERROR_CODE(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        __CASE_ERROR_CODE(CL_OUT_OF_RESOURCES);
        __CASE_ERROR_CODE(CL_OUT_OF_HOST_MEMORY);
        __CASE_ERROR_CODE(CL_PROFILING_INFO_NOT_AVAILABLE);
        __CASE_ERROR_CODE(CL_MEM_COPY_OVERLAP);
        __CASE_ERROR_CODE(CL_IMAGE_FORMAT_MISMATCH);
        __CASE_ERROR_CODE(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        __CASE_ERROR_CODE(CL_BUILD_PROGRAM_FAILURE);
        __CASE_ERROR_CODE(CL_MAP_FAILURE);
        __CASE_ERROR_CODE(CL_MISALIGNED_SUB_BUFFER_OFFSET);
        __CASE_ERROR_CODE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
        __CASE_ERROR_CODE(CL_COMPILE_PROGRAM_FAILURE);
        __CASE_ERROR_CODE(CL_LINKER_NOT_AVAILABLE);
        __CASE_ERROR_CODE(CL_LINK_PROGRAM_FAILURE);
        __CASE_ERROR_CODE(CL_DEVICE_PARTITION_FAILED);
        __CASE_ERROR_CODE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
        __CASE_ERROR_CODE(CL_INVALID_VALUE);
        __CASE_ERROR_CODE(CL_INVALID_DEVICE_TYPE);
        __CASE_ERROR_CODE(CL_INVALID_PLATFORM);
        __CASE_ERROR_CODE(CL_INVALID_DEVICE);
        __CASE_ERROR_CODE(CL_INVALID_CONTEXT);
        __CASE_ERROR_CODE(CL_INVALID_QUEUE_PROPERTIES);
        __CASE_ERROR_CODE(CL_INVALID_COMMAND_QUEUE);
        __CASE_ERROR_CODE(CL_INVALID_HOST_PTR);
        __CASE_ERROR_CODE(CL_INVALID_MEM_OBJECT);
        __CASE_ERROR_CODE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        __CASE_ERROR_CODE(CL_INVALID_IMAGE_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_SAMPLER);
        __CASE_ERROR_CODE(CL_INVALID_BINARY);
        __CASE_ERROR_CODE(CL_INVALID_BUILD_OPTIONS);
        __CASE_ERROR_CODE(CL_INVALID_PROGRAM);
        __CASE_ERROR_CODE(CL_INVALID_PROGRAM_EXECUTABLE);
        __CASE_ERROR_CODE(CL_INVALID_KERNEL_NAME);
        __CASE_ERROR_CODE(CL_INVALID_KERNEL_DEFINITION);
        __CASE_ERROR_CODE(CL_INVALID_KERNEL);
        __CASE_ERROR_CODE(CL_INVALID_ARG_INDEX);
        __CASE_ERROR_CODE(CL_INVALID_ARG_VALUE);
        __CASE_ERROR_CODE(CL_INVALID_ARG_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_KERNEL_ARGS);
        __CASE_ERROR_CODE(CL_INVALID_WORK_DIMENSION);
        __CASE_ERROR_CODE(CL_INVALID_WORK_GROUP_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_WORK_ITEM_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_GLOBAL_OFFSET);
        __CASE_ERROR_CODE(CL_INVALID_EVENT_WAIT_LIST);
        __CASE_ERROR_CODE(CL_INVALID_EVENT);
        __CASE_ERROR_CODE(CL_INVALID_OPERATION);
        __CASE_ERROR_CODE(CL_INVALID_GL_OBJECT);
        __CASE_ERROR_CODE(CL_INVALID_BUFFER_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_MIP_LEVEL);
        __CASE_ERROR_CODE(CL_INVALID_GLOBAL_WORK_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_PROPERTY);
        __CASE_ERROR_CODE(CL_INVALID_IMAGE_DESCRIPTOR);
        __CASE_ERROR_CODE(CL_INVALID_COMPILER_OPTIONS);
        __CASE_ERROR_CODE(CL_INVALID_LINKER_OPTIONS);
        __CASE_ERROR_CODE(CL_INVALID_DEVICE_PARTITION_COUNT);
        __CASE_ERROR_CODE(CL_INVALID_PIPE_SIZE);
        __CASE_ERROR_CODE(CL_INVALID_DEVICE_QUEUE);
        default: return;
    }

    m_error_string.erase(m_error_string.begin(), m_error_string.begin() + 3);
    boost::algorithm::replace_all(m_error_string, "_", " ");
    boost::algorithm::to_lower(m_error_string);
}
