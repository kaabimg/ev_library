#pragma once

#include <ev/core/basic_types.hpp>
#include <ev/core/flags.hpp>
#include <string>
#include <functional>

namespace ev {
namespace net {

class context_t;
class message_t;

namespace socket_id {
static const char* localhost = "localhost";
std::string tcp(const char* host, uint16_t port);
std::string inproc(const char* name);
std::string ipc(const char* name);
}

enum class socket_opts_e : int {
    none = 0,
    dont_wait = 1,
    send_more = 2
};

EV_FLAGS(socket_opts_e)


class socket_t : non_copyable_t {
public:

    using receive_handler_type = std::function<void(message_t&&)>;
    socket_t(context_t&, int type);
    ~socket_t();
    socket_t(socket_t&&);
    socket_t& operator=(socket_t&&);

    /////

    void connect(const char*);
    void disconnect(const char*);

    void bind(const char*);
    void unbind(const char*);

    /////

    void send(const message_t&, flags_t<socket_opts_e> flags = socket_opts_e::none);
    bool receive(flags_t<socket_opts_e> flags = socket_opts_e::none);

    void set_receive_handler(const receive_handler_type&);
    void set_receive_handler(receive_handler_type&&);

    void set_receive_timeout(int timepout_ms);
    int reveive_timeout()const;
protected:
    void* socket() const;

private:
    void* m_socket = nullptr;
    receive_handler_type m_receive_handler = nullptr;
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
    server_t(context_t&);
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

    /////////

    void accept(const void *filter = nullptr, size_t size = 0);
    void unaccept(const void *filter = nullptr, size_t size = 0);

};

class pusher_t : public socket_t {
};

class puller_t : public socket_t {
};
}
}
