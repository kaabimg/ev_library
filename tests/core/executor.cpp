#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ev/core/executor.hpp>
#include <atomic>

TEST_CASE("executor_tc")
{
    std::atomic_uint counter(0);
    {
        ev::executor executor;
        for (int i = 0; i < 100; ++i) {
            executor.async_detached([&] { counter++; });
        }
    }

    REQUIRE(counter == 100);
}
