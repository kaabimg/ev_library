
#include "function.hpp"
#include "private_data.hpp"

using namespace ev::vm::jit;

void block::set_as_insert_point()
{
    d->function->module->context->builder.SetInsertPoint(d->data);
}

void block::set_return(const value& val)
{
    d->function->module->context->builder.CreateRet(val.data());
}

value block::new_variable(const type& type, const std::string& name)
{
}

bool block::has_variable(const std::string& name) const
{
    auto iter = std::find_if(d->variables.begin(), d->variables.end(),
                             [&](auto& i) { return i.name == name; });
    return iter != d->variables.end();
}

value block::find_variable(const std::string& name) const
{
    value res;
    auto iter = std::find_if(d->variables.rbegin(), d->variables.rend(),
                             [&](auto& i) { return i.name == name; });

    if (iter != d->variables.rend()) res = iter->val;
    return res;
}

std::string function::logical_name() const
{
    return d->data->getName();
}

const function_creation_info& function::creation_info() const
{
    return d->creation_info;
}

type function::return_type() const
{
    return type{d->data->getReturnType()};
}

size_t function::arg_count() const
{
    return d->creation_info.arg_names.size();
}

type function::arg_type_at(size_t i) const
{
    auto arg_iterator = d->data->arg_begin();
    while (i--) {
        ++arg_iterator;
    }
    return type(arg_iterator->getType());
}

block function::new_block(const std::string& name)
{
    block blk = create_object<block>();
    blk.d->function = d.get();
    blk.d->data = llvm::BasicBlock::Create(d->module->context->llvm_context, name, d->data);
    return blk;
}

bool function::finalize(std::string* error_str)
{
    if (error_str) {
        llvm::raw_string_ostream error_stream(*error_str);
        if (llvm::verifyFunction(*d->data, &error_stream)) {
            error_stream.flush();
            return false;
        }
        return true;
    }
    else {
        return !llvm::verifyFunction(*d->data);
    }
}

function::operator function_data() const
{
    return d->data;
}

value function::find_variable(const std::string& name) const
{
    auto iter = std::find_if(d->data->arg_begin(), d->data->arg_end(),
                             [&](auto& arg) { return arg.getName() == name; });

    if (iter != d->data->arg_end()) {
        return create_object<value>(d->module->context, &(*iter));
    }
    return value();
}

bool function::has_parameter(const std::string& name) const
{
    auto iter = std::find_if(d->data->arg_begin(), d->data->arg_end(),
                             [&](auto& arg) { return arg.getName() == name; });

    return iter != d->data->arg_end();
}

uintptr_t function::compiled_function() const
{
    return d->function_ptr;
}

bool function::operator==(const function& another) const
{
    return d->creation_info == another->creation_info && d->module == another->module;
}
