#ifndef EV_VM_JIT_MODULE_H
#define EV_VM_JIT_MODULE_H

#include <memory>
#include "object.h"

namespace ev { namespace vm { namespace jit {

class module_private_t;
class function_t;
class function_id_t;
class compilation_scope_t;
class function_creation_info_t;
class value_t;

struct module_t : object_t<module_private_t> {

    function_t new_function(const function_creation_info_t &);
    function_t find_function(const function_id_t &)const;

    void push_scope(compilation_scope_t&);
    void pop_scope();
    compilation_scope_t& current_scope();

    value_t find_variable(const std::string & name) const ;



private:
    friend class context_t;
};

}}}


#endif//EV_VM_JIT_MODULE_H
