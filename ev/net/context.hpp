#pragma once
#include <ev/core/basic_types.hpp>

namespace ev {
namespace net {

class context_t : non_copyable {
public:
    context_t();
    ~context_t();
    context_t(context_t&&);
    context_t& operator=(context_t&&);

    //////

    std::size_t io_thread_count() const;
    void set_io_thread_count(std::size_t count);

    //////

private:
    void* d = nullptr;
    friend class socket_t;
};
}
}
