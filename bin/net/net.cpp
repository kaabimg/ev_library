//#include <ev/core/logging_helpers.hpp>
#include <ev/core/logging.hpp>

#include <ev/core/executor.hpp>

#include <ev/net/context.hpp>
#include <ev/net/socket.hpp>
#include <ev/net/message.hpp>

#include <zmq.hpp>

using namespace ev::net;

//ev::executor_t printer{1};

//void server()
//{
//    zmq::context_t context;

//    zmq::socket_t socket{context, zmq::socket_type::rep};
//    socket.bind("tcp://*:5555");

//    zmq::message_t message;
//    socket.recv(&message);

//    std::string str((char*)message.data(), message.size());

//    ev::debug() << "Server received " << str;

//    zmq::message_t reply(str.size());

//    std::copy(str.begin(), str.end(), (char*)reply.data());

//    socket.send(reply);
//}

//void client()
//{
//    zmq::context_t context;
//    zmq::socket_t socket{context, zmq::socket_type::req};
//    socket.connect("tcp://localhost:5555");

//    std::string str = "hello";

//    zmq::message_t message(str.size());
//    std::copy(str.begin(), str.end(), (char*)message.data());

//    ev::debug() << "Client sending" << str;
//    socket.send(message);

//    socket.recv(&message);

//    str = std::string((char*)message.data(), message.size());

//    ev::debug() << "Client received " << str;
//}

//void publisher()
//{
//    printer << [] { ev::debug() << "Publisher will start in 5 seconds ..."; };

//    std::this_thread::sleep_for(std::chrono::seconds(5));
//    zmq::context_t context;

//    zmq::socket_t socket{context, zmq::socket_type::pub};
//    socket.bind("tcp://*:5555");

//    int i = 0;
//    std::string str;

//    printer << [] { ev::debug() << "Publisher started"; };

//    ev_forever
//    {
//        str = "message_" + std::to_string(i++);
//        socket.send(str.data(), str.size());

//        std::this_thread::sleep_for(std::chrono::seconds(1));
//    }
//}

//void subscriber(size_t id)
//{
//    zmq::context_t context;

//    zmq::socket_t socket{context, zmq::socket_type::sub};
//    socket.connect("tcp://localhost:5555");
//    socket.setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);

//    ev_forever
//    {
//        zmq::message_t message;
//        socket.recv(&message);
//        std::string str((char*)message.data(), message.size());
//        printer << [str, id] { ev::debug() << "Received" << str << "in suscriber" << id; };
//    }
//}

int main()
{
    //    auto pub  = std::async(publisher);
    //    auto sub1 = std::async(subscriber, 1);
    //    auto sub2 = std::async(subscriber, 2);
    //    auto sub3 = std::async(subscriber, 3);

    //    pub.get();
    //    sub1.get();
    //    sub2.get();
    //    sub3.get();

    auto pub_f = std::async([] {
        ev::net::context_t context;
        ev::net::publisher_t publisher = context.make_publisher();
        publisher.bind("tcp://*:5555");

    });

    auto sub_f = std::async([] {
        ev::net::context_t context;
        ev::net::subscriber_t subscriber = context.make_subscriber();
        subscriber.connect("tcp://localhost:5555");
        subscriber.start();



    });

    pub_f.get();
    sub_f.get();





}
