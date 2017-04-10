#pragma once

namespace ev {
namespace net {

namespace detail {
template <typename T>
struct bind_wrapper_t {
    inline void bind(const char* addr)
    {
        reinterpret_cast<T>T::_bind(addr);
    }
    inline void unbind(const char* addr)
    {
        T::_unbind(addr);
    }
};
template <typename T>
struct connect_wrapper_t {
    inline void connect(const char* addr)
    {
        T::_connect(addr);
    }
    inline void disconnect(const char* addr)
    {
        T::_disconnect(addr);
    }
};
}
}
}
