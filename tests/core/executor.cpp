#define BOOST_TEST_MODULE executor
#include <boost/test/included/unit_test.hpp>

#include <ev/core/executor.hpp>
#include <atomic>

BOOST_AUTO_TEST_CASE( executor )
{
    std::atomic_uint counter(0);
    {
        ev::executor executor;
        for (int i = 0; i < 100; ++i) {
            executor.async_detached([&] { counter++; });
        }
    }

    BOOST_REQUIRE(counter == 100);
}
