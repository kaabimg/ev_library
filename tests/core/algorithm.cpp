#define BOOST_TEST_MODULE algorithm
#include <boost/test/included/unit_test.hpp>

#include <ev/core/algorithm.hpp>

BOOST_AUTO_TEST_CASE(tuple)
{
    auto tuple = std::make_tuple(1, 2.5, std::string("hello"));

    ev::for_each(tuple, [](auto& elem) { elem = elem + elem; });

    BOOST_CHECK_EQUAL(std::get<0>(tuple), 2);
    BOOST_CHECK_EQUAL(std::get<1>(tuple), 5.);
    BOOST_CHECK_EQUAL(std::get<2>(tuple), "hellohello");
}

BOOST_AUTO_TEST_CASE(structure)
{
    struct s3 {
        int i = 1, j = 2, k = 3;
    };

    s3 s;
    static_assert(ev::arity<s3>() == 3);

    ev::for_each(ev::members_of(s), [](auto& mem) { mem *= 2; });

    BOOST_CHECK_EQUAL(s.i, 2);
    BOOST_CHECK_EQUAL(s.j, 4);
    BOOST_CHECK_EQUAL(s.k, 6);
}
