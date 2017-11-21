#define BOOST_TEST_MODULE scope_guard
#include <boost/test/included/unit_test.hpp>

#include <ev/core/scope_guard.hpp>

BOOST_AUTO_TEST_CASE(scope_guard)
{
    int i = 0;
    {
        on_scope(exit)
        {
            i = 4;
        };
    }
    BOOST_REQUIRE(i == 4);

    int j = 0;
    try {
        on_scope(failure)
        {
            j = 4;
        };
        throw 2;
    }
    catch (...) {
    }
    BOOST_REQUIRE(j == 4);

    int k = 0;
    try {
        on_scope(success)
        {
            k = 4;
        };
    }
    catch (...) {
    }
    BOOST_REQUIRE(k == 4);

    int l = 0;
    try {
        int g = 45;

        conditional_scope_exit(g % 2)
        {
            l = 4;
        };
    }
    catch (...) {
    }
    BOOST_REQUIRE(l == 4);
}
