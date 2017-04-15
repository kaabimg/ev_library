#pragma once
#include <ev/core/basic_types.hpp>

struct zmq_msg_t;
namespace ev {
namespace net {

namespace detail {

typedef struct zmq_msg_base_t {
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || \
    (defined(__SUNPRO_C) && __SUNPRO_C >= 0x590) || (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x590)
    unsigned char _[64] __attribute__((aligned(sizeof(void*))));
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    __declspec(align(8)) unsigned char _[64];
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_ARM_ARMV7VE))
    __declspec(align(4)) unsigned char _[64];
#else
    unsigned char _[64];
#endif
} zmq_msg_base_t;
}

class message_t : non_copyable_t {
public:
    message_t(std::size_t size = 0);

    message_t(const message_t&);
    message_t(message_t&&);

    message_t& operator =(const message_t&);
    message_t& operator =(message_t&&);

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
