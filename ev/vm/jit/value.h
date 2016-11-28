#pragma once


#include "data_fwd_declare.h"
#include "object.h"

namespace ev { namespace vm { namespace jit {

struct value_private_t;
struct type_t;

struct value_t : object_t<value_private_t>
{

    type_t type()const;
    bool is_number()const;

    value_data_t data()const;



    value_t operator+(const value_t& another);
    value_t operator-(const value_t& another);
    value_t operator-();
    value_t operator/(const value_t& another);
    value_t operator*(const value_t& another);


    value_t cast_to(const type_t&)const;





protected:
    std::pair<value_t,value_t> cast_types(const value_t & v1,const value_t & v2);

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


