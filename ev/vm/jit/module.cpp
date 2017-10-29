#include "module.hpp"

#include "context.hpp"
#include "function.hpp"
#include "function_signature.hpp"
#include "private_data.hpp"
#include "type.hpp"

using namespace ev::vm;
using namespace ev::vm::jit;

void module::dump()
{
    d->module.dump();
}

std::string module::name() const
{
    return d->module.getName().str();
}

function module::new_function(const function_creation_info& info)
{
    function func = create_object<function>();
    func.d->creation_info = info;
    func.d->module = d.get();
    std::vector<type_data> args_type(info.arg_types.size());

    int i = 0;
    for (auto& type_name : info.arg_types) {
        args_type[i++] = find_type(type_name);
    }

    llvm::FunctionType* ft = llvm::FunctionType::get(find_type(info.return_type), args_type, false);

    func.d->data =
        llvm::Function::Create(ft, llvm::Function::ExternalLinkage, info.name, &d->module);
    i = 0;
    for (auto& arg : func.d->data->args()) {
        arg.setName(info.arg_names[i++]);
    }

    d->functions.push_back(func);

    return func;
}

void module::remove_function(function& f)
{
    f.d->data->removeFromParent();

    auto iter = std::find(d->functions.begin(), d->functions.end(), f);
    d->functions.erase(iter);
}

function module::find_function(const function_id& info) const
{
    auto iter = std::find_if(d->functions.begin(), d->functions.end(),
                             [&](auto& f) { return info == f.creation_info(); });

    if (iter != d->functions.end()) return *iter;
    return function();
}

function module::find_function(const std::string& name, unsigned arg_count) const
{
    auto iter = std::find_if(d->functions.begin(), d->functions.end(), [&](const function& f) {
        return (name == f.creation_info().name) &&
               (f.creation_info().arg_names.size() == arg_count);
    });

    if (iter != d->functions.end()) return *iter;
    return function();
}

const std::vector<function>& module::functions() const
{
    return d->functions;
}

std::vector<function>& module::functions()
{
    return d->functions;
}

structure module::new_struct(structure_info&& info)
{
    std::vector<type_data> types{info.field_types.size()};

    int i = 0;
    for (auto& type : info.field_types) types[i++] = type;

    llvm::StructType* type = llvm::StructType::create(types);
    type->setName(info.name);

    structure stct = create_object<structure>();

    stct->data = type;
    stct->module = d.get();
    d->structs_data[type] = std::move(info);
    return stct;
}

structure module::find_struct(const std::string& name) const
{
    auto s_iter = d->structs.find(name);
    if (s_iter != d->structs.end()) return s_iter->second;
    return structure();
}

value module::new_call(const function& f, std::vector<value_data>&& args)
{
    return create_object<value>(d->context, d->context->builder.CreateCall(f, std::move(args)));
}

void module::push_scope(compilation_scope& scope)
{
    d->scope_stack.push_back(&scope);
}

void module::pop_scope()
{
    d->scope_stack.pop_back();
}

compilation_scope& module::current_scope()
{
    return *(*d->scope_stack.rbegin());
}

type_kind module::kind_of(const std::string& name) const
{
    auto iter = builtin_type_ids.find(name);
    if (iter != builtin_type_ids.end()) return iter->second;

    auto s_iter = d->structs.find(name);
    if (s_iter != d->structs.end()) return type_kind::structure;
    return type_kind::unknown;
}

type module::find_type(const std::string& name) const
{
    type_kind kind = kind_of(name);
    if (kind == type_kind::unknown) return type();
    if (kind == type_kind::structure) return find_struct(name).to_type();
    return d->context->interface->get_builtin_type(kind);
}

value module::find_variable(const std::string& name) const
{
    value var;
    for (auto i = d->scope_stack.rbegin(); i != d->scope_stack.rend(); ++i) {
        var = (*i)->find_variable(name);
        if (var) return var;
    }
    return var;
}
