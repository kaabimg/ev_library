#include "module.h"

#include "context.h"
#include "function.h"
#include "utils.h"
#include "private_data.h"


using namespace ev::vm::jit;

std::string module_t::name() const
{
    return d->module.getName().str();
}

function_t module_t::new_function(const function_creation_info_t& info)
{
    function_t func = create_object<function_t>();
    func.d->creation_info = info;
    func.d->module = d.get();
    std::vector<type_data_t> args_type(info.args_type.size());

    int i = 0;
    for(auto type_id : info.args_type) args_type[i++] = d->context->interface->get_type(type_id);

    llvm::FunctionType* ft = llvm::FunctionType::get(
                d->context->interface->get_type(info.return_type),
                args_type,
                false
                );


    func.d->data = llvm::Function::Create(
                ft,
                llvm::Function::ExternalLinkage,
                info.name,
                &d->module
                );
    i = 0;
    for (auto & arg : func.d->data->args()) {
        arg.setName(info.args_names[i++]);
    }

    d->functions.push_back(func);

    return func;
}

function_t module_t::find_function(const function_id_t & info) const
{
    auto iter = std::find_if(
                d->functions.begin(),d->functions.end(),
                [&](auto & f){return info == f.info();}
    );

    if(iter != d->functions.end()) return *iter;
    return function_t();
}


value_t module_t::new_call(const function_t & f, const std::vector<value_data_t> & args)
{
    return create_object<value_t>(d->context,d->context->builder.CreateCall(f,args));
}


void module_t::push_scope(compilation_scope_t & scope)
{
    d->scope_stack.push_back(&scope);
}

void module_t::pop_scope()
{
    d->scope_stack.pop_back();
}

compilation_scope_t& module_t::current_scope()
{
    return *(*d->scope_stack.rbegin());
}

value_t module_t::find_variable(const std::string &name) const
{
    value_t var;
    for (auto i = d->scope_stack.rbegin(); i != d->scope_stack.rend(); ++i) {
        var = (*i)->find_variable(name);
        if(var) return var;
    }
    return var;
}
