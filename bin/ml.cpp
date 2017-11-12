
//#include <ev/ml/neural_network.hpp>
//#include <ev/ml/algorithm.hpp>
#include <ev/core/logging_helpers.hpp>
//#include <ev/core/overloaded.hpp>

//#include <ev/core/traits.hpp>
#include <ev/core/allocator.hpp>
#include <ev/core/meta.hpp>

int main()
{
//    //    auto input_layer = ev::ml::basic_layer(2);
//    //    auto layer1 = ev::ml::make_layer<ev::ml::sigmoid_t>(3);
//    //    auto output_layer = ev::ml::make_layer<ev::ml::sigmoid_t>(1);

//    //    ev::ml::connect(input_layer, layer1);
//    //    ev::ml::connect(layer1, output_layer);

//    //    input_layer.update_from({1.0, 2.0});
//    //    ev::ml::feed_forward(ev::ml::sigmoid, input_layer, layer1, output_layer);

//    //    output_layer.apply([](auto value, auto&&) { ev::debug() << value; });

//    ev::arena<double, 32> arena;
//    ev::stack_allocator<double, 32> allocator(arena);

//    ev::debug() << arena.begin()[0] << arena.begin()[1] << arena.begin()[2] << arena.begin()[3];

//    std::vector<double, ev::stack_allocator<double, 32>> vec(allocator);
//    size_t element_count = 8;

//    ev::debug() << "arena begin" << arena.begin();

//    ev::debug() << "---- element count" << element_count;

//    vec.reserve(element_count);

//    for (size_t i = 0; i < element_count; ++i) {
//        ev::debug() << "capacity" << i << ":" << vec.capacity();
//        ev::debug() << "ptr" << vec.data();
//        vec.push_back(i);
//    }

//    ev::debug() << arena.begin() << vec.data() << arena.contains(vec.data());

//    ev::debug() << "****************** printing arena";
//    for (size_t i = 0; i < element_count; ++i) {
//        ev::debug() << arena.begin()[i];
//    }

    auto t = ev::type<const int&&>();

    ev::debug() << "is_pointer" << t.is_pointer();
    ev::debug() << "is_reference" << t.is_reference();
    ev::debug() << "is_const" << t.is_const();

//    t.debug();
}
