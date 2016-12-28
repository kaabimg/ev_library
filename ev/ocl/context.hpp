#pragma once

#include "common.hpp"
namespace ev { namespace ocl {

class device_t;


class context_t : object_wrapper_t<cl_context>{
public:
    context_t(const std::vector<device_t>& devices);

};

}}
