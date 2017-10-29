#pragma once
#include <ev/core/basic_types.hpp>

namespace ev {
namespace net {

class io_thread_t : non_copyable {
public:
    io_thread_t();
    ~io_thread_t();

    void start();
    void stop();
};
}
}
