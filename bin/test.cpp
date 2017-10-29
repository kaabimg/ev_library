#include <ev/core/logging_helpers.hpp>
#include <ev/core/preprocessor.hpp>
#include <ev/core/executor.hpp>

#include <vector>
#include <future>
#include <string_view>
#include <any>
#include <variant>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

namespace asio = boost::asio;

int main()
{
    asio::io_service io_servive;
    auto work_guard = std::make_unique<asio::io_service::work>(io_servive);

    io_servive.post([&] {
        ev::debug() << __PRETTY_FUNCTION__;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        work_guard.reset();
    });

    std::thread run_thread{[&] { io_servive.run(); }};

    run_thread.join();
}
