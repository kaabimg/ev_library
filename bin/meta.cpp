#include <ev/meta/meta.hpp>
#include <boost/hana.hpp>
#include <ev/core/logging_helpers.hpp>
#include <ev/core/overloaded.hpp>

namespace hana = boost::hana;

struct foo {
    bool b = false;
    int i = 10;
    float f = 3.;
    double d = 563.56;
    std::string str = "hello";
    std::vector<std::string> str_vec = {"1", "2", "3"};
};

BOOST_HANA_ADAPT_STRUCT(foo, b, i, f, d, str, str_vec);

int main()
{
    auto visitor = ev::make_overloaded(
        [](bool val) { ev::debug() << val; }, [](int val) { ev::debug() << val; },
        [](float val) { ev::debug() << val; }, [](double val) { ev::debug() << val; },
        [](const std::string& val) { ev::debug() << val; },
        [](const std::vector<std::string>& val) { ev::debug() << val; });
    auto value_provider =
        ev::make_overloaded([](int) { return 11; }, [](float) { return float(11.); },
                         [](double) { return double(11); }, [](const std::string&) { return "11"; },
                         [](const std::vector<std::string>&) -> std::vector<std::string> {
                             return {"11", "11"};
                         });

    foo f;

    hana::for_each(f, [&](auto&& pair) {
        ev::debug() << hana::to<const char*>(hana::first(pair)) << hana::second(pair);
    });

    hana::for_each(f, [&](auto&& pair) {
        ev::debug() << "writing" << hana::to<const char*>(hana::first(pair))
                    << value_provider(hana::second(pair));
        hana::second(pair) = value_provider(hana::second(pair));
    });

    hana::for_each(f, [&](auto&& pair) {
        ev::debug() << hana::to<const char*>(hana::first(pair)) << hana::second(pair);
    });
}
