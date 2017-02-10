#define CATCH_CONFIG_MAIN

#include <ev/core/synchronized_data.hpp>
#include <thread>
#include <catch.hpp>

TEST_CASE("synchronized_data_tc")
{
    ev::synchronized_data_t<std::vector<int>> sv;
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

    REQUIRE(sv->empty());
}
