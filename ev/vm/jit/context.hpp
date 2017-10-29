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
class context_private;
class function;
class value;
class module;
class compilation_scope;
class function_id;
class type;
class structure;
class structure_info;

struct context : non_copyable
{
    context();
    ~context();

    template <typename T>
    value new_constant(T value);

    type get_builtin_type(type_kind kind);

    module new_module(const std::string& name);
    module find_module(const std::string& name) const;
    module main_module() const;

    void compile();

protected:
    value new_constant(type_kind kind, void* val);

private:
    context_private* d;
    friend class function;
    friend class value;
    friend class block;
};

template <typename T>
value context::new_constant(T value)
{
    return new_constant(get_type_kind<T>(), &value);
}
}
}
}
