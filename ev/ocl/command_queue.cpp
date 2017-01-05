#include "command_queue.hpp"
#include "action.hpp"
#include "context.hpp"
#include "device.hpp"

using namespace ev::ocl;

command_queue_t::command_queue_t(const context_t& context,
                                 const device_t& device)
    : wrapper_type{nullptr} {
    cl_int status;
    cl_queue_properties properties = 0;

    set_data(clCreateCommandQueue(context.cl_object(), device.cl_object(),
                                  properties, &status),
             init_mode_e::create);

    check_status(status);
}

command_queue_t& command_queue_t::operator<<(const flush_t&) {
    check_status(clFinish(cl_object()));
    return *this;
}

command_queue_t& command_queue_t::operator<<(const action_t& action) {
    enqueue(action);
    return *this;
}

event_t command_queue_t::enqueue(const action_t& action,
                                 const std::vector<event_t>& wait_list) {
    return action.exec(*this, wait_list);
}
