#include "context.hpp"
#include "private_data.hpp"
#include "type.hpp"

using namespace ev::vm::jit;

context_t::context_t() : d(new context_private_t) {
    d->interface = this;
    new_module("main");
}

context_t::~context_t() {
    delete d;
}

void context_t::compile() {
    if (d->has_compiled_modules) {
        d->execution_engine.remove(d->added_modules_handle);
    } else {
        d->has_compiled_modules = true;
    }
    std::vector<llvm::Module*> modules{d->modules.size()};
    int i = 0;
    for (auto& module : d->modules) { modules[i++] = &module.second.d->module; }

    d->added_modules_handle = d->execution_engine.add(std::move(modules));

    for (auto& module : d->modules) {
        ev::debug() << "-------------------------------------";
        module.second->module.dump();

        for (function_t& f : module.second.functions()) {
            f->function_ptr =
                d->execution_engine.find_symbol(f.logical_name()).getAddress();
        }
    }
}

type_t context_t::get_builtin_type(type_kind_e kind) {
    type_t type;
    switch (kind) {
        case type_kind_e::boolean:
            type.m_data = llvm::Type::getInt1Ty(d->context);
            break;
        case type_kind_e::i32:
            type.m_data = llvm::Type::getInt32Ty(d->context);
            break;
        case type_kind_e::i64:
            type.m_data = llvm::Type::getInt64Ty(d->context);
            break;
        case type_kind_e::r32:
            type.m_data = llvm::Type::getFloatTy(d->context);
            break;
        case type_kind_e::r64:
            type.m_data = llvm::Type::getDoubleTy(d->context);
            break;
        default: break;
    }
    return type;
}

value_t context_t::new_constant(type_kind_e kind, void* val) {
    value_t value = create_object<value_t>();
    value->context = d;

    switch (kind) {
        case type_kind_e::boolean:
            value->data =
                llvm::ConstantInt::get(llvm::Type::getInt8Ty(d->context),
                                       *reinterpret_cast<bool*>(val));
            break;
        case type_kind_e::i32:
            value->data =
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(d->context),
                                       *reinterpret_cast<std::int32_t*>(val));
            break;
        case type_kind_e::i64:
            value->data =
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(d->context),
                                       *reinterpret_cast<std::int64_t*>(val));
            break;
        case type_kind_e::r32:
            value->data = llvm::ConstantFP::get(
                d->context, llvm::APFloat(*reinterpret_cast<float*>(val)));

            break;
        case type_kind_e::r64:
            value->data = llvm::ConstantFP::get(
                d->context, llvm::APFloat(*reinterpret_cast<double*>(val)));
            break;

        default: break;
    }

    return value;
}

module_t context_t::new_module(const std::string& name) {
    if (find_module("name")) return module_t();

    module_t module = create_object<module_t>(name, d);
    module->context = d;

    d->modules.insert({name, module});

    return module;
}

module_t context_t::find_module(const std::string& name) const {
    auto iter = d->modules.find(name);
    if (iter != d->modules.end()) return iter->second;
    return module_t();
}

module_t context_t::main_module() const {
    return find_module("main");
}
