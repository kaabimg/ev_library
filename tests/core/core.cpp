#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ev/core/scope_exit.hpp>
#include <ev/core/logging.hpp>

TEST_CASE("scope_exit_tc")
{
    int i = 0;
    {
        on_scope_exit { i = 4; };
    }
    REQUIRE(i == 4);

    int j = 0;
    try
    {
        on_scope_exit_with_exception { j = 4; };
        throw 2;
    }
    catch (...)
    {
    }
    REQUIRE(j == 4);

    int k = 0;
    try
    {
        on_scope_exit_without_exception { k = 4; };
    }
    catch (...)
    {
    }
    REQUIRE(k == 4);

    int l = 0;
    try
    {
        int g = 45;

        conditional_scope_exit(g % 2) { l = 4; };
    }
    catch (...)
    {
    }
    REQUIRE(l == 4);
}

#include <ev/core/synchronized_data.hpp>
#include <ev/core/thread_pool.hpp>

TEST_CASE("synchronized_data_tc")
{
    ev::synchronized_data_t<std::vector<int>> sv;
    int size           = 100;
    auto producer_task = [&]() mutable {

        for (int i = 0; i < size; ++i) {
            sv->push_back(i);
        }

    };
    auto consumer_task = [&]() mutable {
        int total = 0;

        while (total != size) {
            synchronized(sv)
            {
                if (!sv.empty()) {
                    sv.pop_back();
                    ++total;
                }
            }
        }
    };

    std::thread producer{producer_task}, consumer{consumer_task};
    producer.join();
    consumer.join();

    REQUIRE(sv->empty());
}

TEST_CASE("thread_pool_tc")
{
    ev::synchronized_data_t<std::vector<int>> sv;
    ev::synchronized_data_t<double, std::mutex> sn(10.4);
    {
        ev::thread_pool_t th_p{4};
        int size           = 100;
        auto producer_task = [&]() mutable { sv->push_back(rand()); };
        auto consumer_task = [&]() mutable {

            bool exit = false;

            while (!exit) {
                synchronized(sv)
                {
                    if (!sv.empty()) {
                        sv.pop_back();
                        exit = true;
                    }
                }
                synchronized(sn) { sn = 12.; }
            }
        };

        for (int i = 0; i < size; ++i) {
            th_p.post_detached(producer_task);
            th_p.post_detached(consumer_task);
            th_p.post_detached(consumer_task);
            th_p.post_detached(producer_task);
        }

        std::future<void> last = th_p.post([] {});
        last.wait();
    }

    REQUIRE(sv->empty());
    REQUIRE(sn.copy() == 12.);
}

#include <ev/core/flags.hpp>

enum class flags_tc_e
{
    zero = 0,
    one  = 1,
    two  = 2,
    foor = 4
};
EV_FLAGS(flags_tc_e)

TEST_CASE("flags_tc")
{
    ev::flags_t<flags_tc_e> res = flags_tc_e::one | flags_tc_e::two;

    REQUIRE(res.test(flags_tc_e::one));
    REQUIRE(res.test(flags_tc_e::two));
    REQUIRE(!res.test(flags_tc_e::foor));

    res.clear(flags_tc_e::two);

    REQUIRE(!res.test(flags_tc_e::two));

    res.enable(flags_tc_e::foor);

    REQUIRE(res.test(flags_tc_e::foor));
}
