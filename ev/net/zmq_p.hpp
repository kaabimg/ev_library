#pragma once

#include <zmq.h>
#include <stdexcept>

namespace ev {
namespace net {
namespace detail {

inline void throw_if_eror(int ret)
{
    if (ret == -1) throw std::runtime_error(zmq_strerror(zmq_errno()));
}

}
}
}
