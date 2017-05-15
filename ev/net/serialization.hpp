#pragma once

#include "socket.hpp"

#include <ev/core/logging.hpp>
#include <ev/core/basic_types.hpp>

#include <string>
#include <cstring>



namespace ev {
namespace net {

template <typename T>
struct serializer_t {
    static void write(socket_t& socket, const T& d)
    {
        socket.send(&d, sizeof(T));
    }
};

template <>
struct serializer_t<const char*> {
    static void write(socket_t& socket, const char* d)
    {
        socket.send(d, std::strlen(d));
    }
};

template <>
struct serializer_t<std::string> {
    static void write(socket_t& socket, const std::string& d)
    {
        socket.send(d.c_str(), d.size());
    }
};


class socket_writer_t {
public:
    socket_writer_t(socket_t& s) : m_socket(s)
    {
    }

    template <typename T>
    socket_writer_t& operator<<(T&& d)
    {
        serializer_t<typename std::decay<T>::type>::write(m_socket, std::forward<T>(d));
        return *this;
    }

private:
    socket_t& m_socket;
};

}
}
