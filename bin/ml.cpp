#include <ev/core/logging_helpers.hpp>
#include <ev/core/execution_flow.hpp>

static std::mutex mutex;

void print(auto... args)
{
    std::lock_guard g{mutex};
    (ev::debug() << ... << args);
}

int main()
{
    using ev::exf::column;
    using ev::exf::graph;
    using ev::exf::node;

    graph g;
    node<double(double)> input{g, 2};
    input.set_task([](double i) {
        print("In input", std::this_thread::get_id());
        return i * 2;
    });

    ev::exf::node<double(double)> processing1{g, 2}, processing2{g, 2};

    auto task = [](double i) {
        print("In processing", std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::microseconds(400));
        return i * i;
    };

    processing1.set_task(task);
    processing2.set_task(task);

    node<void(double)> output{g};
    auto grab_result = [](double val) {
        print("In grab_result", val, std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    };
    output.set_task(grab_result);

    input >> column{processing1, processing2} >> output;

    input(3);

    g.wait_all();
}
