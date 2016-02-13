#include "jit_types.h"

using namespace ev::vm;


///////////////////////////////////////////
///////////////////////////////////////////
//////  context


context_t::context_t(){
    m_jit_contex = jit_context_create();

    create_main_function();

}

context_t::~context_t(){
    jit_context_destroy(m_jit_contex);
}


function_t context_t::create_function(const ast::function_declaration_t& func_decl)
{
    jit_function_t jit_function;

    jit_type_t jit_signature;
    size_t parameters_count = func_decl.arguments.size();

    function_meta_data_t * meta_data = new function_meta_data_t();
    meta_data->name = func_decl.name.value;
    meta_data->arguments_names.resize(parameters_count);
    meta_data->signature.return_type = get_builtin_type<double>();
    meta_data->signature.arguments_types = std::vector<jit_type_t> (parameters_count,get_builtin_type<double>());

    size_t i = 0;
    for(const ast::identifier_t & id : func_decl.arguments){
        meta_data->arguments_names[i++] = id.value;
    }


    jit_signature = jit_type_create_signature(
                jit_abi_cdecl,
                meta_data->signature.return_type,
                meta_data->signature.arguments_types.data(),
                parameters_count,
                1
                );

    jit_function = jit_function_create(m_jit_contex, jit_signature);

    jit_type_free(jit_signature);

    function_t function(jit_function);
    function.set_meta_data(meta_data);

    return function;
}

function_t context_t::create_expression_function()
{
    jit_type_t jit_signature;

    function_meta_data_t * meta_data = new function_meta_data_t();
    meta_data->name = "expression_function";
    meta_data->signature.return_type = get_builtin_type<double>();

    jit_signature = jit_type_create_signature(
                jit_abi_cdecl,
                meta_data->signature.return_type,
                nullptr,
                0,// parmaeters count
                1
                );

    jit_function_t jit_function = jit_function_create(m_jit_contex, jit_signature);

    jit_type_free(jit_signature);

    function_t function(jit_function);
    function.set_meta_data(meta_data);
    return function;
}

function_t context_t::create_variable_declaration_function()
{
    jit_type_t jit_signature;

    function_meta_data_t * meta_data = new function_meta_data_t();
    meta_data->name = "variable_declare_function";
    meta_data->signature.return_type = get_builtin_type<void>();

    jit_signature = jit_type_create_signature(
                jit_abi_cdecl,
                meta_data->signature.return_type,
                nullptr,
                0,// parmaeters count
                1
                );

    jit_function_t jit_function = jit_function_create(m_jit_contex, jit_signature);

    jit_type_free(jit_signature);

    function_t function(jit_function);
    function.set_meta_data(meta_data);
    return function;
}




void context_t::create_main_function()
{

    jit_type_t jit_signature;

    function_meta_data_t * meta_data = new function_meta_data_t();
    meta_data->name = "main";


    jit_signature = jit_type_create_signature(
                jit_abi_cdecl,
                get_builtin_type<void>(),
                nullptr,
                0,// parmaeters count
                1
                );

    jit_function_t jit_function = jit_function_create(m_jit_contex, jit_signature);

    jit_type_free(jit_signature);

    function_t main_function(jit_function);
    main_function.set_meta_data(meta_data);

    main_function.set_recompilable(true);

    m_main_scope = std::make_unique<scope_t>(main_function,nullptr);

    m_current_scope = m_main_scope.get();
}

void context_t::handle_exception(int )
{

}


void context_t::push(const function_t& func)
{
    scope_t scope{func,m_current_scope};
    m_current_scope->sub_scopes.push_back(scope);
    m_current_scope = & (*m_current_scope->sub_scopes.rbegin());
}

void context_t::push(const block_t& block)
{
    scope_t scope{block,m_current_scope};
    m_current_scope->sub_scopes.push_back(scope);
    m_current_scope = & (*m_current_scope->sub_scopes.rbegin());
}

void context_t::pop()
{
    m_current_scope = m_current_scope->parent_scope;
}

function_t context_t::current_function() const
{
    scope_t * scope = m_current_scope;

    while (scope) {
        if(scope->is_function()){
            return scope->data.function;
        }
        scope = scope->parent_scope;
    }

    assert(false && "should at least return main function");
    return function_t();
}

function_t context_t::main_function() const
{
    return m_main_scope->data.function;
}

scope_t& context_t::current_scope()
{
    return *m_current_scope;
}

scope_t&context_t::global_scope()
{
    return *m_main_scope.get();
}


value_t context_t::create_constant( double val)
{
    return jit_value_create_float64_constant(
                main_function().data(),
                get_builtin_type<double>(),
                val
                );
}



///////////////////////////////////////////
///////////////////////////////////////////
//////  function

const char* function_t::name() const
{
    return meta_data().name.c_str();
}

bool function_t::matches(const std::string& name, const function_signature_t& signature) const
{
    return (meta_data().name == name) && (meta_data().signature == signature);
}

bool function_t::matches(const function_signature_t& signature) const
{
    return meta_data().signature == signature;
}

void function_t::abandon()
{
    jit_function_abandon(m_jit_function);
    m_jit_function = nullptr;
}



value_t function_t::get_parmeter(const std::string& name)const
{
    size_t size = meta_data().arguments_names.size();
    for (size_t i = 0; i < size; ++i) {
        if(name == meta_data().arguments_names[i])
        {
            return jit_value_get_param(m_jit_function,i);
        }
    }

    return value_t();
}

value_t function_t::create_variable()
{
    return jit_value_create(m_jit_function, get_builtin_type<double>());

}

value_t function_t::create_constant(double val)
{
    return jit_value_create_float64_constant(
                m_jit_function,
                get_builtin_type<double>(),
                val);
}

value_t function_t::add(const value_t& v1, const value_t& v2)
{
    return jit_insn_add(m_jit_function,v1.data(),v2.data());
}

value_t function_t::sub(const value_t& v1, const value_t& v2)
{
    return jit_insn_sub(m_jit_function,v1.data(),v2.data());
}

value_t function_t::mul(const value_t& v1, const value_t& v2)
{
    return jit_insn_mul(m_jit_function,v1.data(),v2.data());
}

value_t function_t::div(const value_t& v1, const value_t& v2)
{
    return jit_insn_div(m_jit_function,v1.data(),v2.data());

}

value_t function_t::negative(const value_t& v)
{
    return jit_insn_neg(m_jit_function,v.data());
}

void function_t::store(const value_t& dest, const value_t& source)
{
    jit_insn_store(m_jit_function,dest.data(),source.data());
}


value_t function_t::create_function_call(const function_t& func,const std::vector<jit_value_t>& args)
{

    return jit_insn_call(
                m_jit_function,
                func.name(),
                func.data(),
                0,
                const_cast<jit_value_t*>(args.data()),
                args.size(),
                0
                );
}

void function_t::set_return(const value_t& v)
{
    jit_insn_return(m_jit_function,v.data());
}

bool function_t::compile()
{
    return jit_function_compile(m_jit_function);
}

bool function_t::is_compiled() const
{
    return jit_function_is_compiled(m_jit_function);
}

void function_t::set_recompilable(bool recompilable)
{
    if(recompilable){
        jit_function_set_recompilable(m_jit_function);
    }
    else {
        jit_function_clear_recompilable(m_jit_function);
    }
}

bool function_t::is_recopmilable() const
{
    return jit_function_is_recompilable(m_jit_function);
}

void* function_t::to_closure()
{
    return jit_function_to_closure(m_jit_function);
}

void function_t::clear_meta_data(void* md)
{
    delete reinterpret_cast<function_meta_data_t*>(md);
}

void function_t::set_meta_data(function_meta_data_t* md)
{
    jit_function_set_meta(m_jit_function,0,md,&clear_meta_data,0);
}

const function_meta_data_t& function_t::meta_data() const
{
    return *reinterpret_cast<function_meta_data_t*>(jit_function_get_meta(m_jit_function,0));
}

size_t function_t::args_count() const
{
    return meta_data().signature.arguments_types.size();
}


///////////////////////////////////////////
///////////////////////////////////////////
/// value_t

jit_type_t value_t::type() const
{
    return jit_value_get_type(m_jit_value);
}

function_t value_t::function() const
{
    return jit_value_get_function(m_jit_value);
}

value_t value_t::load() const
{
    return jit_insn_load(jit_value_get_function(m_jit_value),m_jit_value);
}

value_t value_t::load_relative() const
{
    return jit_insn_load_relative(
                jit_value_get_function(m_jit_value),
                m_jit_value,
                0,
                type()
                );

}

void value_t::store(const value_t & value)
{
    jit_insn_store(function().data(),m_jit_value,value.data());
}

void value_t::store_relative(const value_t& value)
{
    jit_insn_store_relative(function().data(),m_jit_value,0,value.data());
}

///////////////////////////////////////////
///////////////////////////////////////////
///  scope_t
function_t scope_t::enclosing_function() const
{
    if(is_function()){
        return data.function;
    }

    if(parent_scope){
        return parent_scope->enclosing_function();
    }

    assert(false && "no global function defined");

    return function_t();
}


void scope_t::register_function(const function_t& func)
{
    data.functions.push_back(func);
}

void scope_t::register_variable(const std::string& name, const value_t& v)
{
    data.variables[name] = v;

}

value_t scope_t::find_variable(const std::string& name) const
{
    std::unordered_map<std::string,value_t>::const_iterator i = data.variables.find(name);

    if(i != data.variables.end()){
        return i->second;
    }
    else {
        if(is_function()){
            value_t value = data.function.get_parmeter(name);
            if(value) return value;
        }

        if(parent_scope){
            return parent_scope->find_variable(name);
        }
        return value_t();
    }
}

function_t scope_t::find_function(const std::string& name, const function_signature_t& signature) const
{
    for(const function_t & function : data.functions){
        if(function.matches(name,signature)){
            return function;
        }
    }

    if(is_function() && data.function.matches(name,signature)){
        return data.function;
    }

    if(parent_scope){
        return parent_scope->find_function(name,signature);
    }


    return function_t();
}

value_t scope_t::create_variable(const std::string& name, double initial_value)
{
    value_t variable = jit_value_create(enclosing_function().data(),get_builtin_type<double>());
    value_t value = enclosing_function().create_constant(initial_value);
    variable.store(value);

    data.variables[name] = variable;

    return variable;
}

double*scope_t::create_host_variable(const std::string& name, double value)
{
    data.host_variables[name] = value;
    return &data.host_variables[name];
}

const double*scope_t::find_host_variable(const std::string& name) const
{
    std::unordered_map<std::string,double>::const_iterator i =  data.host_variables.find(name);

    if(i != data.host_variables.end()){
        return &(i->second);
    }
    return nullptr;
}
