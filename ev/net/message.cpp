#include "message.hpp"
#include "zmq_p.hpp"

using namespace ev::net;

message_t::message_t(std::size_t size)
{
    d = new zmq_msg_t();
    zmq_msg_init(d);
    if (size) resize(size);
}

message_t::~message_t()
{
    if (d) {
        zmq_msg_close(d);
        delete d;
    }
}

zmq_msg_t* message_t::take()
{
    zmq_msg_t* tmp = d;
    d = nullptr;
    return tmp;
}

message_t::message_t(message_t&& another)
{
    std::swap(d, another.d);
}

message_t& message_t::operator=(message_t&& another)
{
    std::swap(d, another.d);
    return *this;
}

void message_t::resize(std::size_t size)
{
    int ret = zmq_msg_init_size(d, size);
    detail::throw_if_eror(ret);
}

std::size_t message_t::size() const
{
    return zmq_msg_size(d);
}

void* message_t::data()
{
    return zmq_msg_data(d);
}

const void* message_t::data() const
{
    return zmq_msg_data(d);
}

void message_t::write(const void* data, std::size_t size)
{
    if (this->size() != size) resize(size);

    std::copy((byte_t*)data, ((byte_t*)data) + size, (byte_t*)this->data());
}
