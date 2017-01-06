#pragma once

#include <ev/core/logging_helpers.hpp>

#include "ocl.hpp"

EV_CUSTOM_PRINTER(ev::ocl::platform_t) {
    ostream << "platfom (" << data.name() << ',' << data.cl_object() << ")";
}

EV_CUSTOM_PRINTER(ev::ocl::device_t) {
    ostream << "device (" << data.name() << ',' << data.cl_object() << ")";
}

EV_CUSTOM_PRINTER(ev::ocl::context_t) {
    ostream << "context (" << data.cl_object() << ")";
}

EV_CUSTOM_PRINTER(ev::ocl::program_t) {
    ostream << "program (" << data.cl_object() << ")";
}

EV_CUSTOM_PRINTER(ev::ocl::kernel_t) {
    ostream << "kernel (" << data.name() << ',' << data.cl_object() << ")";
}
EV_CUSTOM_PRINTER(ev::ocl::command_queue_t) {
    ostream << "command queue (" << data.cl_object() << ")";
}
