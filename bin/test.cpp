#include <ev/core/logging_helpers.hpp>
#include <ev/core/preprocessor.hpp>

#include <vector>
#include <future>
#include <string_view>
#include <any>
#include <variant>
#include <thread>
#include <ev/core/visitor.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
namespace asio = boost::asio;

int main()
{
    auto visitor =
        ev::make_visitor([](int val) { ev::debug() << __PRETTY_FUNCTION__ << val; },
                         [](float val) { ev::debug() << __PRETTY_FUNCTION__ << val; },
                         [](const std::string& val) { ev::debug() << __PRETTY_FUNCTION__ << val; });

    std::variant<int, float, std::string> var;

    var = 2.3f;

    std::visit(visitor, var);

    ev::debug() << std::this_thread::get_id();
    asio::io_service io_context;
    auto work_guard = std::make_unique<asio::io_service::work>(io_context);

    asio::deadline_timer action_timer{io_context, boost::posix_time::seconds(1)};
    action_timer.async_wait([&](const boost::system::error_code&) {
        io_context.post([] { ev::debug() << std::this_thread::get_id(); });
    });

    asio::deadline_timer exit_timer{io_context, boost::posix_time::seconds(5)};
    exit_timer.async_wait([&](const boost::system::error_code&) {
        io_context.post([&] {
            ev::debug() << "exiting";
            work_guard.reset();
        });
    });

    std::thread thread{[&] { io_context.run(); }};

    thread.join();
}
