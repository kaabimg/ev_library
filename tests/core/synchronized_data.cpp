#define BOOST_TEST_MODULE synchronized_data
#include <boost/test/included/unit_test.hpp>
#include <ev/core/synchronized_data.hpp>
#include <thread>

BOOST_AUTO_TEST_CASE(synchronized_data)
{
    ev::synchronized_data<std::vector<int>> sv;
    const int size     = 100;
    auto producer_task = [&]() mutable {

        for (int i = 0; i < size; ++i) {
            sv->push_back(i);
        }

    };
    auto consumer_task = [&]() mutable {
        int total = 0;

        while (total != size) {
            synchronized(sv)
            {
                if (!sv.empty()) {
                    sv.pop_back();
                    ++total;
                }
            }
        }
    };

    std::thread producer{producer_task}, consumer{consumer_task};
    producer.join();
    consumer.join();

    BOOST_REQUIRE(sv->empty());
}
