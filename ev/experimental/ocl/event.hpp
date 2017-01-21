#pragma once

#include "common.hpp"

namespace ev
{
namespace ocl
{
EV_OCL_DECLARE_CLEAR_FUNCTION(cl_event, clRetainEvent, clReleaseEvent)

class event_t : public object_wrapper_t<cl_event>
{
public:
    event_t(cl_event event);

    void wait();
};
}
}
