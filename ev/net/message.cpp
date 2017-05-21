#include "message.hpp"
#include "zmq_p.hpp"

using namespace ev::net;

static_assert(sizeof(zmq_msg_t) == sizeof(detail::zmq_msg_base_t) &&
                  alignof(zmq_msg_t) == alignof(detail::zmq_msg_base_t),
              "ZQM message definition changed");

message_t::message_t(std::size_t size)
{
    detail::zmq_call(zmq_msg_init, msg());

    if (size) resize(size);
}

message_t::message_t(const message_t& another)
{
    detail::zmq_call(zmq_msg_init, msg());
    detail::zmq_call(zmq_msg_copy, msg(), another.msg());
}

message_t::message_t(message_t&& another)
{
    detail::zmq_call(zmq_msg_init, msg());
    detail::zmq_call(zmq_msg_move, msg(), another.msg());
}

message_t& message_t::operator=(const message_t& another)
{
    detail::zmq_call(zmq_msg_close, msg());
    detail::zmq_call(zmq_msg_init, msg());
    detail::zmq_call(zmq_msg_copy, msg(), another.msg());
    return *this;
}

message_t& message_t::operator=(message_t&& another)
{
    detail::zmq_call(zmq_msg_close, msg());
    detail::zmq_call(zmq_msg_init, msg());
    detail::zmq_call(zmq_msg_move, msg(), another.msg());
    return *this;
}

message_t::~message_t()
{
    detail::zmq_call(zmq_msg_close, msg());
}

void message_t::resize(std::size_t size)
{
    detail::zmq_call(zmq_msg_init_size,msg(), size);
}

std::size_t message_t::size() const
{
    return zmq_msg_byte_size(msg());
}

void* message_t::data()
{
    return zmq_msg_data(msg());
}

const void* message_t::data() const
{
    return zmq_msg_data(msg());
}
byte_t
void message_t::write(const void* data, std::size_t size)
{
    if (this->size() != size) resize(size);

    std::copy((std::byte*)data, ((std::byte*)data) + size, (std::byte*)this->data());
}

zmq_msg_t* message_t::msg() const
{
    return const_cast<zmq_msg_t*>(reinterpret_cast<const zmq_msg_t*>(&d));
}
