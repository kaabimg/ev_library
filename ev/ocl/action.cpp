#include "action.hpp"
#include "memory_object.hpp"
#include "command_queue.hpp"
#include "kernel.hpp"

using namespace ev::ocl;


write_buffer_action_t::write_buffer_action_t(const buffer_t &buffer, void *data, size_t size):
    buffuer_action_data_t {buffer.cl_object(),data,size}
{}

event_t write_buffer_action_t::exec(command_queue_t& cq, const std::vector<event_t> &wait_list) const
{
    cl_event event;

    cl_event wait_events[wait_list.size()];
    int i=0;
    for(auto & e : wait_list) wait_events[i++] = e.cl_object();

    cl_int status = clEnqueueWriteBuffer(
                cq.cl_object(),
                m_buffer,
                CL_FALSE, // nono blocking
                0, // offset
                m_size,
                m_data,
                wait_list.size(),
                wait_list.empty() ? nullptr : wait_events,
                &event);

    check_status(status);

    return event_t {event};
}

read_buffer_action_t::read_buffer_action_t(const buffer_t &buffer, void *data, size_t size):
    buffuer_action_data_t {buffer.cl_object(),data,size}

{}

event_t read_buffer_action_t::exec(command_queue_t& cq, const std::vector<event_t> &wait_list) const
{
    cl_event event;

    cl_event wait_events[wait_list.size()];
    int i=0;
    for(auto & e : wait_list) wait_events[i++] = e.cl_object();

    cl_int status = clEnqueueReadBuffer(
                cq.cl_object(),
                m_buffer,
                CL_FALSE, // blocking
                0,
                m_size,
                m_data,
                wait_list.size(),
                wait_list.empty() ? nullptr : wait_events,
                &event);

    check_status(status);

    return event_t {event};
}



exec_kernel_action_t::exec_kernel_action_t(const kernel_t& kernel):
    m_kernel(kernel.cl_object())
{

}

event_t exec_kernel_action_t::exec(command_queue_t& cq, const std::vector<event_t> &wait_list) const
{
    cl_event event;

    cl_event wait_events[wait_list.size()];
    int i=0;
    for(auto & e : wait_list) wait_events[i++] = e.cl_object();

    check_status(
        clEnqueueNDRangeKernel(
            cq.cl_object(),
            m_kernel,
            m_range.dimension,
            nullptr,
            m_range.size,
            nullptr,
            wait_list.size(),
            wait_list.empty() ? nullptr : wait_events,
            &event
        )
    );

    return event_t {event};
}

exec_kernel_action_t& exec_kernel_action_t::operator [](const nd_range_t& r)
{
    m_range = r;
    return *this;
}


