#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ev/core/property.hpp>
#include <ev/core/logging.hpp>

TEST_CASE("property")
{
    ev::property<int> p;
    p = 32;

    REQUIRE(p == 32);
    REQUIRE(p.read() == 32);

    p.write(43);

    REQUIRE(p == 43);
    REQUIRE(p.read() == 43);

    std::vector<int> vec = {1, 2, 3};

    int* data = vec.data();

    ev::property<std::vector<int>> vp;

    vp = std::move(vec);

    REQUIRE(vp->data() == data);

    ev::property<std::vector<int>> vp1;
    vp1->push_back(1);
    vp1->push_back(2);
    vp1->push_back(3);
    data = vp1->data();

    ev::property<std::vector<int>> vp2 = std::move(vp1);
    REQUIRE(vp2->data() == data);
}

TEST_CASE("pointer")
{
    struct guard {
        bool& alive;
        guard(bool& a) : alive(a)
        {
            alive = true;
        }
        ~guard()
        {
            alive = false;
        }
    };

    bool alive = false;
    {
        ev::property<guard*> p;
        p = new guard(alive);
    }

    REQUIRE(alive == false);
}

TEST_CASE("comparison")
{
    ev::property<int> i1 = 3;
    ev::property<int> i2 = 4;

    REQUIRE(i1 < i2);
    REQUIRE(i1 < 4);
    REQUIRE(i1 < 4.58);

    i2 = i1;

    REQUIRE(i1 == i2);

    std::string a, b;
    REQUIRE(a == b);

    ev::property<std::string> s1 = "hello";
    ev::property<std::string> s2 = "hello";

    REQUIRE(s1 == s2);

    s2 = "gd";

    REQUIRE(s1 != s2);

    struct foo {
        mutable bool entred = false;
        bool operator==(const foo&) const
        {
            entred = true;
            return false;
        }
    };

    ev::property<foo> f1, f2;

    REQUIRE(!(f1 == f2));
    REQUIRE(f1->entred);
}

TEST_CASE("state_machine")
{
    enum class state { active, inactive };
    ev::property<state> s0{state::inactive}, s1{state::inactive}, s2{state::inactive},
        s3{state::inactive};

    s0.add_observer([&](auto& p) {
        if (p == state::active)
            s1 = state::active;
        else
            s2 = state::active;
    });

    s1.add_observer([&](auto&) { s3 = state::active; });
    s2.add_observer([&](auto&) { s3 = state::inactive; });

    s0 = state::active;
    REQUIRE(*s3 == state::active);

    s0 = state::inactive;
    REQUIRE(*s3 == state::inactive);
}

TEST_CASE("move")
{
    std::vector<int> v = {1, 2, 3};
    auto data = v.data();
    ev::property<std::vector<int>> vp1, vp2;
    vp1 = std::move(v);

    REQUIRE(vp1->data() == data);

    vp2 = std::move(vp1);
    REQUIRE(vp2->data() == data);
}

TEST_CASE("observer")
{
    ev::property<int> p;
    int change_count = 0;
    auto observer = p.add_observer([&](auto& p) { ++change_count; });
    p = 3;
    p = 4;

    REQUIRE(change_count == 2);

    p.remove_observer(observer);

    p = 44;
    REQUIRE(change_count == 2);


}

TEST_CASE("scoped_observer")
{
    ev::property<int> p;
    int change_count = 0;
    {
        auto observer = ev::make_scoped_observer(p, [&](auto&) { ++change_count; });
        p = 45;

        REQUIRE(change_count == 1);
    }

    p = 86;
    REQUIRE(change_count == 1);
}

TEST_CASE("pack")
{
    ev::property<int, ev::single_property_notifier> p;
    int change_count = 0;

    p.set_observer(ev::make_observer_pack([&](auto&&) { ++change_count; },
                                          [&](auto&&) { ++change_count; },
                                          [&](auto&&) { ++change_count; }));

    p = 99;

    REQUIRE(change_count == 3);
}
