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
    ev::execution_graph graph;

    ev::execution_node<double(double)> input{graph, 2};
    input.set_task([](double i) {
        print("In input", std::this_thread::get_id());
        return i * 2;
    });

    ev::execution_node<double(double)> processing1{graph, 2}, processing2{graph, 2};

    auto task = [](double i) {
        print("In processing", std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        return i * i;
    };

    processing1.set_task(task);
    processing2.set_task(task);

    ev::execution_node<void(double)> output{graph};
    auto grab_result = [](double val) {
        print("In grab_result", val, std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    };
    output.set_task(grab_result);

    input >> processing1 >> output;
    input >> processing2 >> output;

    input(3);

    graph.wait_all();

}
