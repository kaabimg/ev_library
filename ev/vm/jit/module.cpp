#include "module.hpp"

#include "context.hpp"
#include "function.hpp"
#include "function_signature.hpp"
#include "private_data.hpp"
#include "type.hpp"

using namespace ev::vm;
using namespace ev::vm::jit;

void module_t::dump()
{
    d->module.dump();
}

std::string module_t::name() const
{
    return d->module.getName().str();
}

function_t module_t::new_function(const function_creation_info_t& info)
{
    function_t func       = create_object<function_t>();
    func.d->creation_info = info;
    func.d->module        = d.get();
    std::vector<type_data_t> args_type(info.arg_types.size());

    int i = 0;
    for (auto& type_name : info.arg_types) {
        args_type[i++] = find_type(type_name);
    }

    llvm::FunctionType* ft =
        llvm::FunctionType::get(find_type(info.return_type), args_type, false);

    func.d->data = llvm::Function::Create(ft, llvm::Function::ExternalLinkage,
                                          info.name, &d->module);
    i = 0;
    for (auto& arg : func.d->data->args()) {
        arg.setName(info.arg_names[i++]);
    }

    d->functions.push_back(func);

    return func;
}

void module_t::remove_function(function_t& f)
{
    f.d->data->removeFromParent();

    auto iter = std::find(d->functions.begin(), d->functions.end(), f);
    d->functions.erase(iter);
}

function_t module_t::find_function(const function_id_t& info) const
{
    auto iter =
        std::find_if(d->functions.begin(), d->functions.end(),
                     [&](auto& f) { return info == f.creation_info(); });

    if (iter != d->functions.end()) return *iter;
    return function_t();
}

function_t module_t::find_function(const std::string& name,
                                   unsigned arg_count) const
{
    auto iter = std::find_if(
        d->functions.begin(), d->functions.end(), [&](const function_t& f) {
            return (name == f.creation_info().name) &&
                   (f.creation_info().arg_names.size() == arg_count);
        });

    if (iter != d->functions.end()) return *iter;
    return function_t();
}

const std::vector<function_t>& module_t::functions() const
{
    return d->functions;
}

std::vector<function_t>& module_t::functions()
{
    return d->functions;
}

struct_t module_t::new_struct(struct_info_t&& info)
{
    std::vector<type_data_t> types{info.field_types.size()};

    int i                                          = 0;
    for (auto& type : info.field_types) types[i++] = type;

    llvm::StructType* type = llvm::StructType::create(types);
    type->setName(info.name);

    struct_t stct = create_object<struct_t>();

    stct->data            = type;
    stct->module          = d.get();
    d->structs_data[type] = std::move(info);
    return stct;
}

struct_t module_t::find_struct(const std::string& name) const
{
    auto s_iter = d->structs.find(name);
    if (s_iter != d->structs.end()) return s_iter->second;
    return struct_t();
}

value_t module_t::new_call(const function_t& f,
                           std::vector<value_data_t>&& args)
{
    return create_object<value_t>(
        d->context, d->context->builder.CreateCall(f, std::move(args)));
}

void module_t::push_scope(compilation_scope_t& scope)
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

type_kind_e module_t::type_kind(const std::string& name) const
{
    auto iter = builtin_type_ids.find(name);
    if (iter != builtin_type_ids.end()) return iter->second;

    auto s_iter = d->structs.find(name);
    if (s_iter != d->structs.end()) return type_kind_e::structure;
    return type_kind_e::unknown;
}

type_t module_t::find_type(const std::string& name) const
{
    type_kind_e kind = type_kind(name);
    if (kind == type_kind_e::unknown) return type_t();
    if (kind == type_kind_e::structure) return find_struct(name).to_type();
    return d->context->interface->get_builtin_type(kind);
}

value_t module_t::find_variable(const std::string& name) const
{
    value_t var;
    for (auto i = d->scope_stack.rbegin(); i != d->scope_stack.rend(); ++i) {
        var = (*i)->find_variable(name);
        if (var) return var;
    }
    return var;
}
