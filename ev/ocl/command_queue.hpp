#pragma once

#include "common.hpp"

namespace ev { namespace ocl {

class context_t;
class device_t;
class action_t;
class flush_t;
class event_t;

EV_OCL_DECLARE_CLEAR_FUNCTION(cl_command_queue,clRetainCommandQueue,clReleaseCommandQueue)

class command_queue_t : public object_wrapper_t<cl_command_queue> {
public:
    command_queue_t(const context_t & context,
                    const device_t &device );

    command_queue_t& operator <<(const action_t &);
    command_queue_t& operator <<(const flush_t &);

    event_t enqueue(const action_t &,const std::vector<event_t>& wait_list = std::vector<event_t>());


};


}}
