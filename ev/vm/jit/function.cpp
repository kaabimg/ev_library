
#include "function.h"
#include "private_data.h"

using namespace ev::vm::jit;


void block_t::set_as_insert_point()
{
    m_data->function->module->builder.SetInsertPoint(m_data->data);
}

void block_t::set_return(value_t val)
{
    m_data->function->module->builder.CreateRet(val);
}

value_t block_t::find_variable(const std::string &name) const
{
    value_t res;
    auto iter  = std::find_if(
                m_data->variables.begin(),m_data->variables.end(),
                [&](auto & i){return i.name == name;}
    );

    if(iter != m_data->variables.end()) res = iter->value;
    return res;
}


const function_creation_info_t& function_t::info() const
{
    return m_data->creation_info;
}

block_t function_t::new_block(const std::string &name)
{
    block_t block = create_object<block_t>();
    block.m_data->function = m_data.get();
    block.m_data->data = llvm::BasicBlock::Create(
                m_data->module->context->context,
                name,
                m_data->data
                );

    return block;
}

bool function_t::finalize()
{
    return !llvm::verifyFunction(*m_data->data);
}

function_t::operator function_data_t() const
{
    return m_data->data;
}



value_t function_t::find_variable(const std::string &name) const
{
    auto iter  = std::find_if(
                m_data->arguments.begin(),m_data->arguments.end(),
                [&](named_value_t & arg){return arg.name == name;}
    );

    if(iter != m_data->arguments.end()) return iter->value;
    return value_t();
}

