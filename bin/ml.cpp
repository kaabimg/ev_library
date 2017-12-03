#include <ev/core/logging_helpers.hpp>
#include <ev/core/algorithm.hpp>
#include <ev/core/traits.hpp>
#include <ev/core/executor.hpp>

#include <future>
struct foo {
    int i, j, k;
    static inline std::string data = "hello";
    int dd()
    {
        return 2;
    }
};

int main()
{
    ev::debug() << "Main thread" << std::this_thread::get_id();

    ev::executor executor{1};
    auto f = executor
                 .async([]() {
                     ev::for_each(ev::members_of(foo()), [](auto&& mem) { ev::debug() << mem; });
                     ev::debug() << "Exec thread" << std::this_thread::get_id();
                     return 42;
                 })
                 .then([](auto f){
                     ev::debug() << "Cont thread" << std::this_thread::get_id();
                     ev::debug() << "Result" << f.get();
                 });

    f.get();
}
