#include "context.h"
#include "private_data.h"



using namespace ev::vm::jit;


context_t::context_t() : d (new context_private_t)
{
    d->interface = this;
    new_module("main");
}

context_t::~context_t()
{
    delete d;
}


void context_t::compile()
{
    for(auto & module : d->modules){
        module.second.m_data->module.dump();
    }
}

type_t context_t::get_type(basic_type_kind_e kind)
{
    type_t type;
    switch (kind) {
    case basic_type_kind_e::none:
        type.m_data = llvm::Type::getVoidTy(d->context);
        break;
    case basic_type_kind_e::boolean:
        type.m_data = llvm::Type::getInt8Ty(d->context);
        break;
    case basic_type_kind_e::i32:
        type.m_data = llvm::Type::getInt32Ty(d->context);
        break;
    case basic_type_kind_e::i64:
        type.m_data = llvm::Type::getInt64Ty(d->context);
        break;
    case basic_type_kind_e::f32:
        type.m_data = llvm::Type::getFloatTy(d->context);
        break;
    case basic_type_kind_e::f64:
        type.m_data = llvm::Type::getDoubleTy(d->context);
        break;
    default:
        break;
    }
    return type;
}



value_t context_t::new_constant(basic_type_kind_e kind, void *val)
{
    value_t value = create_object<value_t>();
    value.m_data->context = d;


    switch (kind) {
    case basic_type_kind_e::boolean:
        value.m_data->data =  llvm::ConstantInt::get(
                    llvm::Type::getInt8Ty(d->context),
                    *reinterpret_cast<bool*>(val)
                    );
        break;
    case basic_type_kind_e::i32:
        value.m_data->data =  llvm::ConstantInt::get(
                    llvm::Type::getInt32Ty(d->context),
                    *reinterpret_cast<std::int32_t*>(val)
                    );
        break;
    case basic_type_kind_e::i64:
        value.m_data->data =  llvm::ConstantInt::get(
                    llvm::Type::getInt64Ty(d->context),
                    *reinterpret_cast<std::int64_t*>(val)
                    );
        break;
    case basic_type_kind_e::f32:
        value.m_data->data =  llvm::ConstantFP::get(
                    d->context,
                    llvm::APFloat(*reinterpret_cast<float*>(val))
                    );

        break;
    case basic_type_kind_e::f64:
        value.m_data->data =  llvm::ConstantFP::get(
                    d->context,
                    llvm::APFloat(*reinterpret_cast<double*>(val))
                    );
        break;

    default:
        break;
    }

    return value;
}






value_t context_t::new_call(const function_t & f, const std::vector<value_data_t> & args)
{
    value_t value = create_object<value_t>();
    value.m_data->context = d;
    value.m_data->data  = d->builder.CreateCall(f,args);
    return value;
}



module_t context_t::new_module(const std::string &name)
{

    if(find_module("name")) return module_t();

    module_t module = create_object<module_t>(name,d);
    module.m_data->context = d;
    module.m_data->name = name;
    d->modules.insert({name,module});

    return module;
}

module_t context_t::find_module(const std::string &name) const
{
    auto iter = d->modules.find(name);
    if(iter != d->modules.end()) return iter->second;
    return module_t();
}

module_t context_t::main_module()const
{
    return find_module("main");
}

function_t context_t::find_function(const function_id_t &id,const std::string &module_name)const
{
    module_t module = find_module(module_name);
    if(module) return module.find_function(id);
    return function_t();
}

