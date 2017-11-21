#define BOOST_TEST_MODULE string_utils
#include <boost/test/included/unit_test.hpp>
#include <ev/core/string_utils.hpp>

BOOST_AUTO_TEST_CASE(string_utils)
{
    auto list = ev::split_str("1,2,3", ',');

    BOOST_REQUIRE(list.size() == 3);
    BOOST_REQUIRE(list[0] == "1");
    BOOST_REQUIRE(list[1] == "2");
    BOOST_REQUIRE(list[2] == "3");

    list = ev::split_str("1,2;3", [](char c) { return c == ',' || c == ';' ; });

    BOOST_REQUIRE(list.size() == 3);
    BOOST_REQUIRE(list[0] == "1");
    BOOST_REQUIRE(list[1] == "2");
    BOOST_REQUIRE(list[2] == "3");
}
