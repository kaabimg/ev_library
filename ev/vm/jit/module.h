#pragma once

#include <memory>
#include <vector>
#include "object.h"
#include "data_fwd_declare.h"
#include "function_signature.h"

namespace ev { namespace vm {

namespace jit {

class module_private_t;
class function_t;
class function_id_t;
class compilation_scope_t;
class function_creation_info_t;
class value_t;
class type_t;
class struct_t;
class struct_info_t;

struct module_t : object_t<module_private_t>
{

    void dump();
    std::string name()const;

    function_t new_function(const function_creation_info_t &);
    void remove_function(function_t&);
    function_t find_function(const function_id_t &)const;
    function_t find_function(const std::string & name,unsigned arg_count)const;

    const std::vector<function_t>& functions()const;
    std::vector<function_t>& functions();

    struct_t new_struct(struct_info_t &&);
    struct_t find_struct(const std::string & name) const;

    value_t new_call(const function_t &, std::vector<value_data_t> && args);

    value_t find_variable(const std::string & name) const ;
    void push_scope(compilation_scope_t&);
    void pop_scope();
    compilation_scope_t& current_scope();

    type_kind_e type_kind(const std::string & name)const;

    type_t find_type(const std::string & name)const;



private:
    friend class context_t;
};

}}}


