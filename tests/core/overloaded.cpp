#define BOOST_TEST_MODULE overloaded
#include <boost/test/included/unit_test.hpp>

#include <ev/core/overloaded.hpp>
#include <ev/core/algorithm.hpp>

BOOST_AUTO_TEST_CASE(overloaded)
{
    std::tuple<int, double, std::string, std::vector<int>> tuple{1, 2.5, "hello", {1, 2}};

    auto vis1 = ev::overloaded{[](int& elem) { elem = elem + elem; },
                               [](double& elem) { elem = elem + elem; },
                               [](std::string& elem) { elem = elem + elem; }};

    auto visitor = ev::overloaded{
        vis1, [vis1](std::vector<int>& vec) { std::for_each(vec.begin(), vec.end(), vis1); }};

    ev::for_each(tuple, visitor);

    std::vector res_vect{2, 4};

    BOOST_CHECK_EQUAL(std::get<0>(tuple), 2);
    BOOST_CHECK_EQUAL(std::get<1>(tuple), 5.);
    BOOST_CHECK_EQUAL(std::get<2>(tuple), "hellohello");
    BOOST_CHECK_EQUAL_COLLECTIONS(std::get<3>(tuple).begin(), std::get<3>(tuple).end(),
                                  res_vect.begin(), res_vect.end());
}
