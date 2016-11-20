#pragma once

#include <memory>
#include <vector>
#include "object.h"
#include "data_fwd_declare.h"

namespace ev { namespace vm { namespace jit {

class module_private_t;
class function_t;
class function_id_t;
class compilation_scope_t;
class function_creation_info_t;
class value_t;

struct module_t : object_t<module_private_t>
{

    std::string name()const;

    function_t new_function(const function_creation_info_t &);
    function_t find_function(const function_id_t &)const;
    value_t new_call(const function_t &, const std::vector<value_data_t> &args);


    void push_scope(compilation_scope_t&);
    void pop_scope();
    compilation_scope_t& current_scope();



    value_t find_variable(const std::string & name) const ;



private:
    friend class context_t;
};

}}}


