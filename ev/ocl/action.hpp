#pragma once

#include "nd_range.hpp"
#include "event.hpp"
namespace ev
{
namespace ocl
{
class command_queue_t;
class buffer_t;
class kernel_t;

struct buffuer_action_data_t
{
    buffuer_action_data_t() {}
    buffuer_action_data_t(cl_mem buffer, void* data, size_t size)
        : m_buffer{buffer}, m_data{data}, m_size{size}
    {
    }

    cl_mem m_buffer = nullptr;

    void* m_data  = nullptr;
    size_t m_size = 0;
};

struct flush_t
{
};

class action_t
{
public:
    virtual event_t exec(command_queue_t&,
                         const std::vector<event_t>& wait_list =
                             std::vector<event_t>()) const = 0;
};

class write_buffer_action_t : public action_t, protected buffuer_action_data_t
{
public:
    write_buffer_action_t(const buffer_t& buffer, void* data, size_t size);
    event_t exec(command_queue_t&,
                 const std::vector<event_t>& wait_list =
                     std::vector<event_t>()) const final;
};

class read_buffer_action_t : public action_t, protected buffuer_action_data_t
{
public:
    read_buffer_action_t(const buffer_t& buffer, void* data, size_t size);
    event_t exec(command_queue_t&,
                 const std::vector<event_t>& wait_list =
                     std::vector<event_t>()) const final;
};

class exec_kernel_action_t : public action_t
{
public:
    exec_kernel_action_t(const kernel_t& kernel);
    event_t exec(command_queue_t&,
                 const std::vector<event_t>& wait_list =
                     std::vector<event_t>()) const final;

    exec_kernel_action_t& operator[](const nd_range_t& r);

private:
    cl_kernel m_kernel;
    nd_range_t m_range;
};
}
}
