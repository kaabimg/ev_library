#include <ev/core/logging_helpers.hpp>
#include <ev/core/execution_flow.hpp>

int main()
{

    ev::execution_graph graph;

    ev::execution_node<double(double)> n1 {graph};
    n1.set_task([](double i){
        ev::debug() << "In n1" << std::this_thread::get_id();
        return i * 2;
    });


    ev::execution_node<double(double)> n2 {graph};

    n2.set_task([](double i){
        ev::debug() << "In n2" << std::this_thread::get_id();
        return i * i;
    });

    auto grab_result = [](double val)
    {
        ev::debug() << "In grab_result" << std::this_thread::get_id();
        ev::debug() << "Result" << val;
    };

    n1 >> n2 >> grab_result;

    n1(3);

    graph.wait_all();

}
