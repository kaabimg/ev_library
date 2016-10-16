#include <iostream>

#include <core/variant_serialization.h>
#include <core/logging.h>

template <typename Type>
void test_variant (const Type & val){
    ev::core::variant_t v;
    v = val;
    ev::core::debug() <<"type :" << ev::core::variant_type_name(v)<<":"<<sizeof(v)<<sizeof(double);
}

int main()
{
    test_variant(ev::core::variant_t());
    test_variant(3);
    test_variant(3.4);
    test_variant("hello");

    ev::core::variant_map_t map ;



    map["1"] = 2;
    map["2"] = "hello";

    test_variant(map);



    ev::core::variant_array_t array ;
    array << 1 << 2.1 << "hello" << map << array ;


    test_variant(array);




    return 0;
}

