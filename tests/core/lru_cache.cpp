#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ev/core/lru_cache.hpp>

TEST_CASE("lru_cache")
{
    ev::lru_cache_t<int, std::string> cache{2};

    auto d1 = cache.insert(4, "4");

    auto d2 = cache.get(4);

    REQUIRE(d1.get() == d2.get());

    cache.insert(5, "5");

    REQUIRE(cache.contains(4));
    REQUIRE(cache.contains(5));

    cache.insert(6, "6");

    REQUIRE_FALSE(cache.contains(4));

    cache.get(5);

    cache.insert(7, "7");

    REQUIRE_FALSE(cache.contains(6));
    REQUIRE(cache.contains(5));
}
