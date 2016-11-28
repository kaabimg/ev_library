#pragma once

#include "utils.h"
#include "data_fwd_declare.h"
#include "object.h"

namespace ev { namespace vm { namespace jit {

struct type_t
{
    type_t(type_data_t data) : m_data(data){}
    type_t(){}

    std::string to_string()const;

    type_kind_e kind()const;
    type_data_t data()const {return m_data;}

    operator type_data_t()const {return m_data;}
    operator bool ()const {return m_data;}


    bool is_number()const;
    bool is_integer()const;
    bool is_floating_point()const;


protected:
    type_kind_e int_kind()const;

protected:
    type_data_t m_data = nullptr;
    friend class context_t;
};



struct struct_info_t
{
    std::string name;
    std::vector<type_t> field_types;
    std::vector<std::string> field_names;    
//

    struct_info_t(const std::string & n):name(n){}
    struct_info_t(std::string && n):name(std::move(n)){}
    struct_info_t(){}

    struct_info_t& operator<< (std::pair<type_t,std::string> && );

};


class struct_private_t;

struct struct_t : object_t<struct_private_t>
{
    const std::string& name()const;
    const std::vector<std::string> & field_names()const;
    const std::vector<type_t>& field_types()const;

    int field_index(const std::string & name)const;

    type_t to_type()const;

    friend class module_t;
};


}}}
