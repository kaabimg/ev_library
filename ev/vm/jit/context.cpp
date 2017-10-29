#include "context.hpp"
#include "private_data.hpp"
#include "type.hpp"

using namespace ev::vm::jit;

context::context() : d(new context_private)
{
    d->interface = this;
    new_module("main");
}

context::~context()
{
    delete d;
}

void context::compile()
{
    if (d->has_compiled_modules) {
        d->exec_engine.remove(d->added_modules_handle);
    }
    else {
        d->has_compiled_modules = true;
    }
    std::vector<llvm::Module*> modules{d->modules.size()};
    int i = 0;
    for (auto& module : d->modules) {
        modules[i++] = &module.second.d->module;
    }

    d->added_modules_handle = d->exec_engine.add(std::move(modules));

    for (auto& module : d->modules) {
        ev::debug() << "-------------------------------------";
        module.second->module.dump();

        for (function& f : module.second.functions()) {
            f->function_ptr = d->exec_engine.find_symbol(f.logical_name()).getAddress();
        }
    }
}

type context::get_builtin_type(type_kind kind)
{
    type t;
    switch (kind) {
        case type_kind::boolean: t.m_data = llvm::Type::getInt1Ty(d->llvm_context); break;
        case type_kind::i32: t.m_data = llvm::Type::getInt32Ty(d->llvm_context); break;
        case type_kind::i64: t.m_data = llvm::Type::getInt64Ty(d->llvm_context); break;
        case type_kind::r32: t.m_data = llvm::Type::getFloatTy(d->llvm_context); break;
        case type_kind::r64: t.m_data = llvm::Type::getDoubleTy(d->llvm_context); break;
        default: break;
    }
    return t;
}

value context::new_constant(type_kind kind, void* v)
{
    value val = create_object<value>();
    val->context = d;

    switch (kind) {
        case type_kind::boolean:
            val->data = llvm::ConstantInt::get(llvm::Type::getInt8Ty(d->llvm_context),
                                               *reinterpret_cast<bool*>(v));
            break;
        case type_kind::i32:
            val->data = llvm::ConstantInt::get(llvm::Type::getInt32Ty(d->llvm_context),
                                               *reinterpret_cast<std::int32_t*>(v));
            break;
        case type_kind::i64:
            val->data = llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->llvm_context),
                                               *reinterpret_cast<std::int64_t*>(v));
            break;
        case type_kind::r32:
            val->data =
                llvm::ConstantFP::get(d->llvm_context, llvm::APFloat(*reinterpret_cast<float*>(v)));

            break;
        case type_kind::r64:
            val->data = llvm::ConstantFP::get(d->llvm_context,
                                              llvm::APFloat(*reinterpret_cast<double*>(v)));
            break;

        default: break;
    }

    return val;
}

module context::new_module(const std::string& name)
{
    if (find_module("name")) return module();
    module m = create_object<module>(name, d);
    m->context = d;
    d->modules.insert({name, m});
    return m;
}

module context::find_module(const std::string& name) const
{
    auto iter = d->modules.find(name);
    if (iter != d->modules.end()) return iter->second;
    return module();
}

module context::main_module() const
{
    return find_module("main");
}
