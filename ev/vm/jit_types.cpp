#include "jit_types.h"

using namespace ev::vm;

std::string function_t::mangled_name() const
{
    return jit_code_t::create_mangled_name(m_name,m_signature);
}

bool function_t::matches(const function_signature_t & signature, const std::string &name) const
{
    return m_name == name && m_signature == signature;
}

boost::optional<parameter_t> function_t::find_paramerter(const std::string &name)
{
    auto iter = std::find_if(
                m_parameters.begin(),m_parameters.end(),
                [&](const parameter_t & parameter){return name == parameter.name;}
                );

    if(iter != m_parameters.end())return *iter;
    return {};
}

jit_code_t::jit_code_t(){
    m_jit_context = gccjit::context::acquire();
}

jit_code_t::~jit_code_t(){
    m_jit_context.release();
    if(m_compile_result) gcc_jit_result_release(m_compile_result);
}

function_t jit_code_t::create_function(const ast::function_declaration_t & declaration)
{
    function_t f;
    f.m_name = declaration.name.value;

    f.m_signature.return_type = get_type<double>();

    std::vector<gccjit::param> params;
    auto double_type = get_jit_type<double>(m_jit_context);

    for(auto & arg : declaration.arguments){
        gccjit::param param = m_jit_context.new_param(double_type,arg.value);
        params.push_back(param);
        f.m_parameters.emplace_back(arg.value,param);
        f.m_signature.arguments_types.push_back(get_type<double>());
    }

    gccjit::function func = m_jit_context.new_function(
                GCC_JIT_FUNCTION_EXPORTED,
                double_type,
                f.mangled_name(),
                params,
                false);

    f.m_function = func;
    m_functions.push_back(f);
    return f;
}

boost::optional<function_t> jit_code_t::find_function(
        const function_signature_t & sig,
        const std::string &name) const
{

    auto i = std::find_if(
                m_functions.begin(),m_functions.end(),
                [&](auto & another){return sig == another.m_signature && name == another.m_name;}
    );

    if(i != m_functions.end())return *i;
    return {};
}

void jit_code_t::compile()
{
    if(m_compile_result) gcc_jit_result_release(m_compile_result);
    m_compile_result = m_jit_context.compile();
}

void *jit_code_t::get_compiled_function(const std::string &mangled_name) const
{
    if(m_compile_result){
        return gcc_jit_result_get_code (m_compile_result, mangled_name.c_str());
    }
    return nullptr;
}

std::string jit_code_t::create_mangled_name(const std::string &name, const function_signature_t & signature)
{
    std::string mname;

    static const char* separator = "##";

    mname.append(type_to_string(signature.return_type))
            .append(separator)
            .append(name);

    for(auto type : signature.arguments_types)
        mname.append(separator).append(type_to_string(type));

    return mname;
}

const char* jit_code_t::type_to_string(jit_type_e)
{
    return "double";
}
