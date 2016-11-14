#include "module.h"

#include "context.h"
#include "function.h"
#include "utils.h"
#include "private_data.h"


using namespace ev::vm::jit;

function_t module_t::new_function(const function_creation_info_t& info)
{
    function_t func = create_object<function_t>();
    func.m_data->creation_info = info;
    std::vector<type_data_t> args_type(info.args_type.size());

    int i = 0;
    for(auto type_id : info.args_type) args_type[i++] = m_data->context->interface->get_type(type_id);

    llvm::FunctionType* ft = llvm::FunctionType::get(
                m_data->context->interface->get_type(info.return_type),
                args_type,
                false
                );


    func.m_data->data = llvm::Function::Create(
                ft,
                llvm::Function::ExternalLinkage,
                info.name,
                &m_data->module
                );
    i = 0;
    for (auto & arg : func.m_data->data->args()) {
        arg.setName(info.args_names[i++]);
    }

    m_data->functions.push_back(func);

    return func;
}

function_t module_t::find_function(const function_id_t & info) const
{
    auto iter = std::find_if(
                m_data->functions.begin(),m_data->functions.end(),
                [&](auto & f){return info == f.info();}
    );

    if(iter != m_data->functions.end()) return *iter;
    return function_t();
}

void module_t::push_scope(compilation_scope_t & scope)
{
    m_data->scope_stack.push_back(&scope);
}

void module_t::pop_scope()
{
    m_data->scope_stack.pop_back();
}

compilation_scope_t& module_t::current_scope()
{
    return *(*m_data->scope_stack.rbegin());
}

value_t module_t::find_variable(const std::string &name) const
{
    value_t var;
    for (auto i = m_data->scope_stack.rbegin(); i != m_data->scope_stack.rend(); ++i) {
        var = (*i)->find_variable(name);
        if(var) return var;
    }
    return var;
}
