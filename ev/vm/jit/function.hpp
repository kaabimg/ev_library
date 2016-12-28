#pragma once

#include "function_signature.hpp"
#include "value.hpp"
#include "object.hpp"


namespace ev { namespace vm {

template <typename Sig>
struct runtime_function_t;
struct virtual_machine_t;

namespace jit {

struct block_t;
struct type_t;
struct function_private_t;

struct function_t : object_t<function_private_t>, compilation_scope_t
{
    std::string logical_name()const;
    const function_creation_info_t& creation_info()const;
    type_t return_type()const;
    size_t arg_count()const;
    type_t arg_type_at(size_t )const;

    block_t new_block(const std::string & name);
    bool finalize(std::string* error_str = nullptr);

    operator function_data_t()const;

    value_t find_variable(const std::string & name) const override;
    bool has_parameter(const std::string& )const;

    uintptr_t compiled_function()const;

    bool operator== (const function_t & another)const;


protected:
    friend class module_t;
    friend class context_t;
    friend class ev::vm::virtual_machine_t;

};




struct block_private_t;
struct block_t : object_t<block_private_t>, compilation_scope_t
{
    value_t find_variable(const std::string & name) const override;
    void set_as_insert_point();
    void set_return(const value_t &);

    value_t new_variable(const type_t& type,const std::string&);
    bool has_variable(const std::string& )const;

protected:

    friend class function_t;

};



}}}
