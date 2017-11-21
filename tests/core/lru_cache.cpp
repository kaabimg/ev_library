#define BOOST_TEST_MODULE lru_cache
#include <boost/test/included/unit_test.hpp>

#include <ev/core/lru_cache.hpp>

BOOST_AUTO_TEST_CASE(lru_cache)
{
    ev::lru_cache<int, std::string> cache{2};

    auto d1 = cache.insert(4, "4");

    auto d2 = cache.get(4);

    BOOST_REQUIRE(d1.get() == d2.get());

    cache.insert(5, "5");

    BOOST_REQUIRE(cache.contains(4));
    BOOST_REQUIRE(cache.contains(5));

    cache.insert(6, "6");

    BOOST_REQUIRE(!cache.contains(4));

    cache.get(5);

    cache.insert(7, "7");

    BOOST_REQUIRE(cache.contains(6));
    BOOST_REQUIRE(cache.contains(5));
}
