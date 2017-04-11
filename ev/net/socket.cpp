#include "socket.hpp"
#include "context.hpp"
#include "message.hpp"
#include "zmq_p.hpp"

using namespace ev::net;

///////////////////////////////////////////////////////////////

std::string socket_id::tcp(const char* host, uint16_t port)
{
    std::string str = "tcp://";
    return str.append("tcp://").append(host).append(":").append(std::to_string(port));
}

std::string socket_id::inproc(const char* name)
{
    std::string str = "inproc://";
    return str.append("inproc://").append(name);
}

std::string socket_id::ipc(const char* name)
{
    std::string str = "ipc://";
    return str.append("ipc://").append(name);
}

///////////////////////////////////////////////////////////////

socket_t::socket_t(context_t& context, int type)
{
    d = zmq_socket(context.d, type);
    if (!d) std::runtime_error("Failed to create socket");
}

socket_t::~socket_t()
{
    if (d) zmq_close(d);
}

socket_t::socket_t(socket_t&& another)
{
    std::swap(d, another.d);
}

socket_t& socket_t::operator=(socket_t&& another)
{
    std::swap(d, another.d);
    return *this;
}

void* socket_t::socket() const
{
    return d;
}

//////////////////

void socket_t::bind(const char* addr)
{
    int ret = zmq_bind(d, addr);
    detail::throw_if_eror(ret);
}

void socket_t::unbind(const char* addr)
{
    int ret = zmq_unbind(d, addr);
    detail::throw_if_eror(ret);
}

void socket_t::connect(const char* addr)
{
    int ret = zmq_connect(socket(), addr);
    detail::throw_if_eror(ret);
}

void socket_t::disconnect(const char* addr)
{
    int ret = zmq_disconnect(socket(), addr);
    detail::throw_if_eror(ret);
}

void socket_t::send(const message_t& msg, int flags)
{
    zmq_msg_send(msg.d, d, flags);
}

bool socket_t::receive(message_t& msg, int flags)
{
    int ret = zmq_msg_recv(msg.d, d, flags);
    if (ret != -1) return true;

    int error = zmq_errno();
    if (error == EAGAIN) return false;

    detail::throw_if_eror(-1);
}

void socket_t::set_receive_timeout(int timeout)
{
    int rc = zmq_setsockopt(d, ZMQ_RCVTIMEO, &timeout, sizeof(int));
    detail::throw_if_eror(rc);
}

int socket_t::reveive_timeout() const
{
    int timeout;
    size_t optlen = sizeof(int);
    int rc = zmq_getsockopt(d, ZMQ_RCVTIMEO, &timeout, &optlen);
    detail::throw_if_eror(rc);

    return timeout;
}

///////////////////////////////////////////////////////////////

client_t::client_t(context_t& context) : socket_t(context, ZMQ_REQ)
{
}

///////////////////////////////////////////////////////////////

server_t::server_t(context_t& context) : socket_t(context, ZMQ_REP)
{
}

///////////////////////////////////////////////////////////////

publisher_t::publisher_t(context_t& context) : socket_t(context, ZMQ_PUB)
{
}

///////////////////////////////////////////////////////////////

subscriber_t::subscriber_t(context_t& context) : socket_t(context, ZMQ_SUB)
{
}

void subscriber_t::start()
{
    zmq_setsockopt(socket(), ZMQ_SUBSCRIBE, nullptr, 0);
}

void subscriber_t::stop()
{
}
