#include <ev/core/property.hpp>
#include <ev/core/logging_helpers.hpp>

int main()
{
    auto tuple = std::make_tuple(1, 2, 3);

    ev::for_each(tuple, [&](auto&& e) { ++e; });

    return std::get<0>(tuple) + std::get<1>(tuple) + std::get<2>(tuple);
}
