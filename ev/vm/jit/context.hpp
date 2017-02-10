#pragma once

#include "data_fwd_declare.hpp"
#include "function_signature.hpp"
#include "object.hpp"
#include "value.hpp"

#include <ev/core/basic_types.hpp>

#include <memory>

namespace ev
{
namespace vm
{
namespace jit
{
class context_private_t;
class function_t;
class value_t;
class module_t;
class compilation_scope_t;
class function_id_t;
class type_t;
class struct_t;
class struct_info_t;

struct context_t : non_copyable_t
{
    context_t();
    ~context_t();

    template <typename T>
    value_t new_constant(T value);

    type_t get_builtin_type(type_kind_e kind);

    module_t new_module(const std::string& name);
    module_t find_module(const std::string& name) const;
    module_t main_module() const;

    void compile();

protected:
    value_t new_constant(type_kind_e kind, void* val);

private:
    context_private_t* d;
    friend class function_t;
    friend class value_t;
    friend class block_t;
};

template <typename T>
value_t context_t::new_constant(T value)
{
    return new_constant(get_type_kind<T>(), &value);
}
}
}
}