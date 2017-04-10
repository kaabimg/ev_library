#pragma once
#include <ev/core/basic_types.hpp>

namespace ev {
namespace net {
class publisher_t;
class subscriber_t;
class context_t : non_copyable_t {
public:
    context_t();
    ~context_t();
    context_t(context_t&&);
    context_t& operator=(context_t&&);

    //////

    std::size_t io_thread_count() const;
    void set_io_thread_count(std::size_t count);

    //////


    publisher_t make_publisher();
    subscriber_t make_subscriber();


private:
    void* d = nullptr;
    friend class socket_t;
};
}
}
