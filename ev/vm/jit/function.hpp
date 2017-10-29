#pragma once

#include "function_signature.hpp"
#include "value.hpp"
#include "object.hpp"

namespace ev {
namespace vm {
template <typename Sig>
struct runtime_function;
struct virtual_machine;

namespace jit {
struct block;
struct type;
struct function_private;

struct function : object<function_private>, compilation_scope {
    std::string logical_name() const;
    const function_creation_info& creation_info() const;
    type return_type() const;
    size_t arg_count() const;
    type arg_type_at(size_t) const;

    block new_block(const std::string& name);
    bool finalize(std::string* error_str = nullptr);

    operator function_data() const;

    value find_variable(const std::string& name) const override;
    bool has_parameter(const std::string&) const;

    uintptr_t compiled_function() const;

    bool operator==(const function& another) const;

protected:
    friend class module;
    friend class context;
    friend class ev::vm::virtual_machine;
};

struct block_private;
struct block : object<block_private>, compilation_scope {
    value find_variable(const std::string& name) const override;
    void set_as_insert_point();
    void set_return(const value&);

    value new_variable(const type& type, const std::string&);
    bool has_variable(const std::string&) const;

protected:
    friend class function;
};
}
}
}
