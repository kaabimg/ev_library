
#include "function.h"
#include "private_data.h"

using namespace ev::vm::jit;


void block_t::set_as_insert_point()
{
    d->function->module->context->builder.SetInsertPoint(d->data);
}

void block_t::set_return(const value_t &val)
{
    d->function->module->context->builder.CreateRet(val.data());
}

value_t block_t::new_variable(const type_t &type, const std::string& name)
{

}

bool block_t::has_variable(const std::string & name) const
{
    auto iter  = std::find_if(
                d->variables.begin(),d->variables.end(),
                [&](auto & i){return i.name == name;}
    );
    return iter != d->variables.end();
}

value_t block_t::find_variable(const std::string &name) const
{
    value_t res;
    auto iter  = std::find_if(
                d->variables.rbegin(),d->variables.rend(),
                [&](auto & i){return i.name == name;}
    );

    if(iter != d->variables.rend()) res = iter->value;
    return res;
}


std::string function_t::logical_name() const
{
    return d->data->getName();
}

const function_creation_info_t& function_t::creation_info() const
{
    return d->creation_info;
}

type_t function_t::return_type() const
{
    return type_t {d->data->getReturnType()};
}

size_t function_t::arg_count() const
{
    return d->creation_info.arg_names.size();

}

type_t function_t::arg_type_at(size_t i) const
{
    auto arg_iterator =  d->data->arg_begin();
    while(i--){
        ++arg_iterator;
    }
    return type_t(arg_iterator->getType());
}

block_t function_t::new_block(const std::string &name)
{
    block_t block = create_object<block_t>();
    block.d->function = d.get();
    block.d->data = llvm::BasicBlock::Create(
                d->module->context->context,
                name,
                d->data
                );

    return block;
}

std::pair<bool, std::string> function_t::finalize()
{
    std::pair<bool, std::string> result ;
    llvm::raw_string_ostream error_stream(result.second);
    result.first = !llvm::verifyFunction(*d->data,&error_stream );
    return result;
}

function_t::operator function_data_t() const
{
    return d->data;
}



value_t function_t::find_variable(const std::string &name) const
{
    auto iter  = std::find_if(
                d->data->arg_begin(),d->data->arg_end(),
                [&](auto & arg){return arg.getName() == name;}
    );

    if(iter != d->data->arg_end()) {
        return create_object<value_t>(d->module->context,&(*iter));
    }
    return value_t();
}

bool function_t::has_parameter(const std::string & name) const
{
    auto iter  = std::find_if(
                d->data->arg_begin(),d->data->arg_end(),
                [&](auto & arg){return arg.getName() == name;}
    );

    return iter != d->data->arg_end();
}

uintptr_t function_t::compiled_function() const
{
    return d->function_ptr;
}

bool function_t::operator==(const function_t &another) const
{
    return d->creation_info == another->creation_info &&
            d->module == another->module;
}

