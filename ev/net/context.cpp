#include "context.hpp"
#include "zmq_p.hpp"

using namespace ev::net;

context_t::context_t()
{
    d = zmq_ctx_new();
    if (!d) throw std::runtime_error("Failed to create context");
}

context_t::~context_t()
{
    if (d) zmq_ctx_destroy(d);
}

context_t::context_t(context_t&& another)
{
    std::swap(d, another.d);
}

context_t& context_t::operator=(context_t&& another)
{
    std::swap(d, another.d);
    return *this;
}

std::size_t context_t::io_thread_count() const
{
    return zmq_ctx_get(d,ZMQ_IO_THREADS);
}

void context_t::set_io_thread_count(std::size_t count)
{
    zmq_ctx_set(d, ZMQ_IO_THREADS, count);
}

