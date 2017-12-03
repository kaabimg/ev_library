#define BOOST_TEST_MODULE executor
#include <boost/test/included/unit_test.hpp>

#include <ev/core/logging.hpp>
#include <ev/core/executor.hpp>

#define REPEAT_COUNT 5000

BOOST_AUTO_TEST_CASE(async_detached)
{
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        std::atomic_uint counter(0);
        constexpr uint total = 50;

        ev::executor executor;
        auto task = [&] { ++counter; };

        for (uint i = 0; i < total; ++i) {
            executor << task;
        }

        executor.wait();

        BOOST_REQUIRE_EQUAL((uint)counter, total);
    }
}

BOOST_AUTO_TEST_CASE(continuation)
{
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        std::atomic_uint counter(0);
        ev::executor executor;

        auto task = [&](int id) { counter++; };

        auto f = executor.async(task, 1).then([&](auto) { task(2); });

        f.get();

        BOOST_REQUIRE_EQUAL(counter, 2);
    }
}

BOOST_AUTO_TEST_CASE(wait_executor)
{
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        ev::executor executor;
        auto task = [] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return 42;
        };

        auto f1 = executor.async(task);
        auto f2 = executor.async(task);
        auto f3 = executor.async(task);
        auto f4 = executor.async(task);

        executor.wait();

        BOOST_REQUIRE(f1.is_ready());
        BOOST_REQUIRE(f2.is_ready());
        BOOST_REQUIRE(f3.is_ready());
        BOOST_REQUIRE(f4.is_ready());
    }
}
