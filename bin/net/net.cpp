#include <ev/core/basic_types.hpp>
#include <ev/core/logging.hpp>

#include <ev/core/executor.hpp>
#include <ev/core/property.hpp>

#include <ev/net/context.hpp>
#include <ev/net/serialization.hpp>
#include <ev/net/message.hpp>


using namespace ev::net;

ev::executor_t printer{1};

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
    ev::dynamic_property_t p;

    p = 2.4;

    ev::debug() << p.as<double>() << p.is<double>();


    ev::property_t<std::string> v;
    v.on_modified([&] {
        ev::debug() << "propery changed to " << v.read();
    });

    v = "hello";

    v = "world";

    ev::debug() << (const std::string&)v;

    //    auto pub = std::async(publisher);
    //    auto sub1 = std::async(subscriber, 1);
    //    auto sub2 = std::async(subscriber, 2);
    //    auto sub3 = std::async(subscriber, 3);

    //    pub.get();
    //    sub1.get();
    //    sub2.get();
    //    sub3.get();

    //    auto ser_f = std::async([] {
    //        ev::net::context_t context;
    //        ev::net::server_t server{context};
    //        server.bind("tcp://*:5555");

    //        int i = 0;
    //        ev_forever
    //        {
    //            message_t msg;
    //            server.receive(msg);
    //            ev::debug() << "Server received" << msg.data_as<char>();
    //            msg.write(&i, sizeof(int));
    //            server.send(msg);
    //            i++;
    //        }

    //    });

    //    auto client = [](int id) {
    //        context_t context;
    //        client_t client{context};
    //        client.connect("tcp://localhost:5555");

    //        message_t msg;
    //        msg.write("request", 8);
    //        client.send(msg);

    //        ev::debug() << "reveive_timeout" << client.reveive_timeout();

    //        client.receive(msg);
    //        ev::debug() << "Client received" << id << ":" << (*msg.data_as<int>());

    //    };

    //    auto c1_f = std::async(client, 1);
    //    auto c2_f = std::async(client, 2);

    //    ser_f.get();
    //    c1_f.get();
    //    c2_f.get();
}
