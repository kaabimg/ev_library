#define BOOST_TEST_MODULE pimpl
#include <boost/test/included/unit_test.hpp>

#include "pimpl_class.hpp"

BOOST_AUTO_TEST_CASE(pimpl)
{
    function_type_ptr access_flags;
    {
        pimpl_type obj;
        access_flags = obj.ftype();

        BOOST_REQUIRE(access_flags->test(function_type::default_constructor));
    }
    BOOST_REQUIRE(access_flags->test(function_type::destructor));

    pimpl_type obj;
    pimpl_type obj2{obj};
    access_flags = obj2.ftype();

    BOOST_REQUIRE(access_flags->test(function_type::copy_constructor));

    auto obj2_ptr = obj2.impl_ptr();

    pimpl_type obj3{std::move(obj2)};
    access_flags = obj3.ftype();
    BOOST_REQUIRE(obj3.impl_ptr() == obj2_ptr);
    BOOST_REQUIRE(obj2.impl_ptr() == nullptr);

    obj3 = obj;
    BOOST_REQUIRE(access_flags->test(function_type::copy_assignment));

    auto obj_ptr = obj.impl_ptr();
    obj3 = std::move(obj);
    BOOST_REQUIRE(obj_ptr = obj3.impl_ptr());
}

BOOST_AUTO_TEST_CASE(bimpl)
{
    function_type_ptr access_flags;
    {
        bimpl_type obj;
        access_flags = obj.ftype();
        BOOST_REQUIRE(access_flags->test(function_type::default_constructor));

        bimpl_type obj2{434};
        BOOST_REQUIRE(obj2.ftype()->test(function_type::custom_constructor));

        bimpl_type obj3{obj2};
        BOOST_REQUIRE(obj3.ftype()->test(function_type::copy_constructor));

        bimpl_type tmp;
        bimpl_type obj4{std::move(tmp)};
        BOOST_REQUIRE(obj4.ftype()->test(function_type::move_constructor));

        obj = obj2;
        BOOST_REQUIRE(access_flags->test(function_type::copy_assignment));

        obj = bimpl_type{};
        BOOST_REQUIRE(access_flags->test(function_type::move_assignment));
    }
    BOOST_REQUIRE(access_flags->test(function_type::default_constructor));
}

BOOST_AUTO_TEST_CASE(pod_bimpl)
{
    trivially_destructible_bimpl toto;
    // Should compile
}
