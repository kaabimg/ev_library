#define BOOST_TEST_MODULE flags
#include <boost/test/included/unit_test.hpp>

#include <ev/core/flags.hpp>

enum class flags_tc_e { zero = 0, one = 1, two = 2, foor = 4 };
EV_FLAGS(flags_tc_e)

BOOST_AUTO_TEST_CASE(flags)
{
    ev::flags<flags_tc_e> res = flags_tc_e::one | flags_tc_e::two;

    BOOST_REQUIRE(res.test(flags_tc_e::one));
    BOOST_REQUIRE(res.test(flags_tc_e::two));
    BOOST_REQUIRE(!res.test(flags_tc_e::foor));

    res.clear(flags_tc_e::two);

    BOOST_REQUIRE(!res.test(flags_tc_e::two));

    res.enable(flags_tc_e::foor);

    BOOST_REQUIRE(res.test(flags_tc_e::foor));
}
