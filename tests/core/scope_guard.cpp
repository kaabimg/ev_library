#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ev/core/scope_guard.hpp>

TEST_CASE("scope_guard_tc")
{
    int i = 0;
    {
        on_scope(exit)
        {
            i = 4;
        };
    }
    REQUIRE(i == 4);

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
    REQUIRE(j == 4);

    int k = 0;
    try {
        on_scope(success)
        {
            k = 4;
        };
    }
    catch (...) {
    }
    REQUIRE(k == 4);

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
    REQUIRE(l == 4);
}
