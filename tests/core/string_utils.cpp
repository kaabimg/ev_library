#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ev/core/string_utils.hpp>

TEST_CASE("string_utils")
{
    auto list = ev::split_str("1,2,3", ',');

    REQUIRE(list.size() == 3);
    REQUIRE(list[0] == "1");
    REQUIRE(list[1] == "2");
    REQUIRE(list[2] == "3");

    list = ev::split_str("1,2;3", [](char c) { return c == ',' || c == ';' ; });

    REQUIRE(list.size() == 3);
    REQUIRE(list[0] == "1");
    REQUIRE(list[1] == "2");
    REQUIRE(list[2] == "3");
}
