#pragma once


#include "data_fwd_declare.h"
#include "object.h"

namespace ev { namespace vm { namespace jit {

struct value_private_t;
struct value_t : object_t<value_private_t>
{
    value_t operator+(const value_t& another);
    value_t operator-(const value_t& another);
    value_t operator-();
    value_t operator/(const value_t& another);
    value_t operator*(const value_t& another);


    operator value_data_t()const;

    friend class context_t;
    friend class function_t;
    friend class block_t;
    friend class module_t;
};


struct named_value_t {
    std::string name;
    value_t value;
};


}}}


