#pragma once

#include <zmq.h>
#include <stdexcept>

namespace ev {
namespace net {
namespace detail {

inline void throw_error(int error)
{
    throw std::runtime_error(zmq_strerror(error));
}

inline void throw_if_eror(int ret)
{
    if (ret == -1) throw_error(zmq_errno());
}
template <typename F, typename... Args>
inline void zmq_call(F&& f, Args&&... args)
{
    int error = f(std::forward<Args>(args)...);
    throw_if_eror(error);
}
}
}
}
