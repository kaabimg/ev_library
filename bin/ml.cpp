
#include <ev/ml/neural_network.hpp>
#include <ev/ml/algorithm.hpp>
#include <ev/core/logging_helpers.hpp>
#include <ev/core/overloaded.hpp>

#include <ev/core/traits.hpp>



int main()
{
    auto input_layer = ev::ml::basic_layer(2);
    auto layer1 = ev::ml::make_layer<ev::ml::sigmoid_t>(3);
    auto output_layer = ev::ml::make_layer<ev::ml::sigmoid_t>(1);

    ev::ml::connect(input_layer, layer1);
    ev::ml::connect(layer1, output_layer);

    input_layer.update_from({1.0, 2.0});
    ev::ml::feed_forward(ev::ml::sigmoid, input_layer, layer1, output_layer);

    output_layer.apply([](auto value, auto&&) { ev::debug() << value; });

}
