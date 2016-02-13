#ifndef EV_VARIANT_H
#define EV_VARIANT_H

#include "types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <boost/variant.hpp>

namespace ev { namespace core {


using double_array_t  = std::vector<double>;


enum class variant_type_e {
    null,
    boolean,character,
    i32,ui32,f32,
    i64,ui64,f64,
    pointer,
    string,
    variant_array,
    variant_map,

};



typedef   boost::make_recursive_variant<
                    boost::blank,
                    bool,char,
                    int32,uint32,float,
                    int64,uint64,double,
                    void*,
                    std::string,
                    std::vector<boost::recursive_variant_>,
                    std::unordered_map<std::string,boost::recursive_variant_>
            >::type variant_t ;



using variant_map_t = std::unordered_map<std::string,variant_t>;
using variant_array_t = std::vector<variant_t> ;


inline variant_type_e variant_type(const variant_t & v){
    return static_cast<variant_type_e>(v.which());
}
template <typename T>
inline T & variant_as(variant_t & v){
    return boost::get<T>(v);
}

template <typename T>
inline const T & variant_as(const variant_t & v){
    return boost::get<T>(v);
}

inline const char * variant_type_name(const variant_t & v ){
    switch (variant_type(v)){
        case variant_type_e::null: return "null";
        case variant_type_e::boolean: return "boolean";
        case variant_type_e::character: return "character";
        case variant_type_e::i32: return "i32";
        case variant_type_e::ui32: return "ui32";
        case variant_type_e::f32: return "f32";
        case variant_type_e::i64: return "i64";
        case variant_type_e::ui64: return "ui64";
        case variant_type_e::f64: return "f64";
        case variant_type_e::pointer: return "pointer";
        case variant_type_e::string: return "string";
        case variant_type_e::variant_array: return "variant_array";
        case variant_type_e::variant_map: return "variant_map";
        default: return "invalid";
    }
}


}}


template<typename T>
ev::core::variant_array_t & operator << (
        ev::core::variant_array_t & array,const T & v){
    array.push_back(v);
    return array;
}

template<typename T>
ev::core::variant_array_t & operator << (
        ev::core::variant_array_t & array,T && v){
    array.push_back(std::forward<T>(v));
    return array;
}




#endif // EV_VARIANT_H
