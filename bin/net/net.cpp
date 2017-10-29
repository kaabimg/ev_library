#include <ev/core/basic_types.hpp>
#include <ev/core/logging.hpp>

#include <ev/core/executor.hpp>
#include <ev/core/property.hpp>

#include <ev/net/context.hpp>
#include <ev/net/serialization.hpp>
#include <ev/net/message.hpp>

#include <future>

using namespace ev::net;

ev::executor printer{1};

void publisher()
{
    context_t context;
    publisher_t publisher{context};
    socket_writer_t writer{publisher};

    publisher.bind(socket_id::tcp("*", 5555).c_str());

    printer << [] { ev::debug() << "Publisher will start in 2 seconds ..."; };

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::string str;

    printer << [] { ev::debug() << "Publisher started"; };

    for (int i = 0; i < 10; ++i) {
        str = "message_" + std::to_string(i);
        printer << [str] { ev::debug() << "Sending" << str << "..."; };

        writer << str;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void subscriber(size_t id)
{
    context_t context;

    subscriber_t subscriber{context};
    subscriber.connect(socket_id::tcp("localhost", 5555).c_str());

    message_t message;

    subscriber.set_receive_handler([&message](message_t&& m) { message = std::move(m); });
    subscriber.accept();

    for (int i = 0; i < 10; ++i) {
        if (subscriber.receive()) {
            message_t copy;
            copy = std::move(message);
            std::string str((char*)copy.data(), copy.size());
            printer << [str, id] { ev::debug() << "Received" << str << "in suscriber" << id; };
        }
        else {
            printer << [id] { ev::debug() << "Failed to receive in suscriber" << id; };
        }
    }
}

int main()
{
    std::thread pub {publisher};
    std::thread sub {std::bind(subscriber,1)};

    pub.join();
    sub.join();

}
