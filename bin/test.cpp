#include <ev/core/logging_helpers.hpp>
#include <ev/core/preprocessor.hpp>
#include <ev/core/executor.hpp>

#include <vector>
#include <future>
#include <string_view>
#include <any>
#include <variant>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace asio = boost::asio;

int main()
{
    asio::io_service io_servive;
    asio::ip::tcp::resolver resolver{io_servive};
    asio::ip::tcp::socket socket{io_servive};

    std::vector<char> byte_array(256);

    asio::ip::tcp::resolver::query query{"theboostcpplibraries.com", "80"};

    std::function<void(const boost::system::error_code&, std::size_t)> read_handler = [&](
        const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec)
            ev::error() << "read_handler" << ec.message();
        else {
            ev::debug().write(byte_array.data(), bytes_transferred);
            socket.async_read_some(asio::buffer(byte_array), read_handler);
        }
    };

    auto connect_handler = [&](const boost::system::error_code& ec) {
        if (ec)
            ev::error() << "connect_handler" << ec.message();
        else {
            std::string r = "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
            asio::write(socket, asio::buffer(r));
            socket.async_read_some(asio::buffer(byte_array), read_handler);
        }
    };

    auto resolve_handler = [&](const boost::system::error_code& ec,
                               asio::ip::tcp::resolver::iterator it) {
        if (ec)
            ev::error() << "resolve_handler" << ec.message();
        else
            socket.async_connect(*it, connect_handler);
    };

    resolver.async_resolve(query, resolve_handler);

    io_servive.run();
}
