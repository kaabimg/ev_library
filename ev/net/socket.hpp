#pragma once

#include <ev/core/basic_types.hpp>
#include <string>

namespace ev {
namespace net {

class context_t;
namespace socket_id {
static const char* localhost = "localhost";
std::string tcp(const char* host, uint16_t port);
std::string inproc(const char* name);
std::string ipc(const char* name);
}

class socket_t : non_copyable_t {
public:
    socket_t(context_t&, int type);
    ~socket_t();
    socket_t(socket_t&&);
    socket_t& operator=(socket_t&&);

    socket_t(const socket_t&) = delete;
    socket_t& operator=(const socket_t&) = delete;

    /////

    void connect(const char*);
    void disconnect(const char*);

    void bind(const char*);
    void unbind(const char*);

    /////

protected:
    void* socket() const;

private:
    void* d = nullptr;
};

class client_t : public socket_t {
public:
    client_t(context_t&);
    client_t(client_t&&) = default;
    client_t& operator=(client_t&&) = default;

    /////////
};

class server_t : public socket_t {
public:
    server_t(server_t&);
    server_t(server_t&&) = default;
    server_t& operator=(server_t&&) = default;
};

class publisher_t : public socket_t {
public:
    publisher_t(context_t&);
    publisher_t(publisher_t&&) = default;
    publisher_t& operator=(publisher_t&&) = default;

    /////////
};

class subscriber_t : public socket_t {
public:
    subscriber_t(context_t&);
    subscriber_t(subscriber_t&&) = default;
    subscriber_t& operator=(subscriber_t&&) = default;
};

class pusher_t : public socket_t {
};

class puller_t : public socket_t {
};
}
}
