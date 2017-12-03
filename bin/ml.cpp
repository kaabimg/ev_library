#include <ev/core/logging_helpers.hpp>
#include <ev/core/executor.hpp>
#include <ev/core/synchronized_data.hpp>

int main()
{
    ev::debug() << "Main thread" << std::this_thread::get_id();

    ev::executor executor {2};

    ev::synchronized_data<bool> print_guard;
    auto task = [&]() {

        synchronized(print_guard)
        {
            ev::debug() << "Exec thread" << std::this_thread::get_id();
        }
        return 42;
    };

    executor << task << task << task << task;

    executor.wait();
}
