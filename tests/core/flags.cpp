#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ev/core/flags.hpp>

enum class flags_tc_e { zero = 0, one = 1, two = 2, foor = 4 };
EV_FLAGS(flags_tc_e)

TEST_CASE("flags_tc")
{
    ev::flags<flags_tc_e> res = flags_tc_e::one | flags_tc_e::two;

    REQUIRE(res.test(flags_tc_e::one));
    REQUIRE(res.test(flags_tc_e::two));
    REQUIRE(!res.test(flags_tc_e::foor));

    res.clear(flags_tc_e::two);

    REQUIRE(!res.test(flags_tc_e::two));

    res.enable(flags_tc_e::foor);

    REQUIRE(res.test(flags_tc_e::foor));
}
