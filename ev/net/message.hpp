#pragma once
#include <ev/core/basic_types.hpp>

struct zmq_msg_t;
namespace ev {
namespace net {

namespace detail {

typedef struct zmq_msg_base_t {unsigned char _ [64];} zmq_msg_base_t;
}

class message_t : non_copyable {
public:
    explicit message_t(std::size_t size = 0);

    message_t(const message_t&);
    message_t(message_t&&);

    message_t& operator=(const message_t&);
    message_t& operator=(message_t&&);

    ~message_t();

    void resize(std::size_t size);
    std::size_t size() const;

    void* data();
    const void* data() const;

    template <typename T>
    const T* data_as() const;

    template <typename T>
    T* data_as();

    void write(const void* data, std::size_t size);

protected:
    zmq_msg_t* msg() const;

private:
    detail::zmq_msg_base_t d;
    friend class socket_t;
};

template <typename T>
inline const T* message_t::data_as() const
{
    return static_cast<const T*>(data());
}

template <typename T>
inline T* message_t::data_as()
{
    return static_cast<T*>(data());
}
}
}
