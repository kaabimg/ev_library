#include "event.hpp"

using namespace ev::ocl;

event_t::event_t(cl_event event) : wrapper_type{event} {}

void event_t::wait() {
    cl_event event = cl_object();
    clWaitForEvents(1, &event);
}
