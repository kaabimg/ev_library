#pragma once

#include "object.hpp"
#include "data_fwd_declare.hpp"
#include "function_signature.hpp"

#include <memory>
#include <vector>

namespace ev
{
namespace vm
{
namespace jit
{
class module_private;
class function;
class function_id;
class compilation_scope;
class function_creation_info;
class value;
class type;
class structure;
class structure_info;

struct module : object<module_private>
{
    void dump();
    std::string name() const;

    function new_function(const function_creation_info&);
    void remove_function(function&);
    function find_function(const function_id&) const;
    function find_function(const std::string& name, unsigned arg_count) const;

    const std::vector<function>& functions() const;
    std::vector<function>& functions();

    structure new_struct(structure_info&&);
    structure find_struct(const std::string& name) const;

    value new_call(const function&, std::vector<value_data>&& args);

    value find_variable(const std::string& name) const;
    void push_scope(compilation_scope&);
    void pop_scope();
    compilation_scope& current_scope();

    type_kind kind_of (const std::string& name) const;

    type find_type(const std::string& name) const;

private:
    friend class context;
};
}
}
}
