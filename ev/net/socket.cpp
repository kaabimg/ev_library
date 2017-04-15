#include "socket.hpp"
#include "context.hpp"
#include "message.hpp"
#include "zmq_p.hpp"

using namespace ev::net;

static_assert(ev::as_int(socket_opts_e::dont_wait) == ZMQ_DONTWAIT, "Invalid socket options");
static_assert(ev::as_int(socket_opts_e::send_more) == ZMQ_SNDMORE, "Invalid socket options");

///////////////////////////////////////////////////////////////

std::string socket_id::tcp(const char* host, uint16_t port)
{
    std::string str = "tcp://";
    return str.append(host).append(":").append(std::to_string(port));
}

std::string socket_id::inproc(const char* name)
{
    std::string str = "inproc://";
    return str.append(name);
}

std::string socket_id::ipc(const char* name)
{
    std::string str = "ipc://";
    return str.append(name);
}

///////////////////////////////////////////////////////////////

socket_t::socket_t(context_t& context, int type)
{
    m_socket = zmq_socket(context.d, type);
    if (!m_socket) std::runtime_error("Failed to create socket");
    set_receive_handler([](message_t&&) {});
}

socket_t::~socket_t()
{
    if (m_socket) zmq_close(m_socket);
}

socket_t::socket_t(socket_t&& another)
{
    std::swap(m_socket, another.m_socket);
    std::swap(m_receive_handler, another.m_receive_handler);
}

socket_t& socket_t::operator=(socket_t&& another)
{
    std::swap(m_socket, another.m_socket);
    std::swap(m_receive_handler, another.m_receive_handler);
    return *this;
}

void* socket_t::socket() const
{
    return m_socket;
}

//////////////////

void socket_t::bind(const char* addr)
{
    detail::zmq_call(zmq_bind, m_socket, addr);
}

void socket_t::unbind(const char* addr)
{
    detail::zmq_call(zmq_unbind, m_socket, addr);
}

void socket_t::connect(const char* addr)
{
    detail::zmq_call(zmq_connect, m_socket, addr);
}

void socket_t::disconnect(const char* addr)
{
    detail::zmq_call(zmq_disconnect, m_socket, addr);
}

void socket_t::send(const message_t& msg, flags_t<socket_opts_e> flags)
{
    detail::zmq_call(zmq_msg_send, msg.msg(), m_socket, flags.data());
}

bool socket_t::receive(flags_t<socket_opts_e> flags)
{
    message_t msg;
    int ret = zmq_msg_recv(msg.msg(), m_socket, flags.data());
    if (ret == 0) {
        m_receive_handler(std::move(msg));
        return true;
    }

    int error = zmq_errno();
    if (error == EAGAIN) return false;

    detail::throw_error(error);
}

void socket_t::set_receive_handler(const socket_t::receive_handler_type& handler)
{
    m_receive_handler = handler;
}

void socket_t::set_receive_handler(socket_t::receive_handler_type&& handler)
{
    m_receive_handler = std::move(handler);
}

void socket_t::set_receive_timeout(int timeout)
{
    detail::zmq_call(zmq_setsockopt, m_socket, ZMQ_RCVTIMEO, &timeout, sizeof(int));
}

int socket_t::reveive_timeout() const
{
    int timeout;
    static const size_t optlen = sizeof(int);
    detail::zmq_call(zmq_getsockopt, m_socket, ZMQ_RCVTIMEO, &timeout,
                     const_cast<size_t*>(&optlen));
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

void subscriber_t::accept(const void* filter, size_t size)
{
    detail::zmq_call(zmq_setsockopt, socket(), ZMQ_SUBSCRIBE, filter, size);
}
