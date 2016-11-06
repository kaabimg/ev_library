#ifndef EV_JIT_TYPES_H
#define EV_JIT_TYPES_H

#include <unordered_map>
#include <vector>
#include <list>
#include <functional>
#include <libgccjit++.h>

#include <boost/optional.hpp>

#include <ev/core/preprocessor.h>
#include <ev/core/logging.h>

#include "ast.h"


namespace ev { namespace vm {

using jit_type_e = gcc_jit_types;
using rvalue_t = gccjit::rvalue;
using lvalue_t = gccjit::lvalue;
using context_t = gccjit::context;
using block_t = gccjit::block;



template <typename T>
inline constexpr jit_type_e get_type(){}

#define EV_VM_DECLARE_TYPE(T,JIT_T) \
    template <>\
    inline constexpr jit_type_e get_type<T>(){\
        return JIT_T;\
    }\

EV_VM_DECLARE_TYPE(void  , GCC_JIT_TYPE_VOID)
EV_VM_DECLARE_TYPE(bool  , GCC_JIT_TYPE_BOOL)
EV_VM_DECLARE_TYPE(int   , GCC_JIT_TYPE_INT)
EV_VM_DECLARE_TYPE(double, GCC_JIT_TYPE_DOUBLE)



template <typename T>
inline gccjit::type get_jit_type(gccjit::context & context){
    return context.get_type(get_type<T>());
}



struct function_signature_t {
    jit_type_e return_type = get_type<void>();
    std::vector<jit_type_e> arguments_types;

    bool operator ==(const function_signature_t & another)const {
        return return_type == another.return_type && arguments_types == another.arguments_types;
    }
};


namespace detail {
inline void build_signature(function_signature_t &,size_t){}

template <typename H>
inline void build_signature(function_signature_t & signature,size_t pos){
    signature.arguments_types[pos] = get_type<H>();
}
template <typename H,typename H2,typename ...T>
inline void build_signature(function_signature_t & signature,size_t pos){
    signature.arguments_types[pos] = get_type<H>();
    build_signature<H2,T...>(signature,++pos);
}


template <typename R,typename ...Args>
inline function_signature_t build_signature(){
    function_signature_t signature;
    signature.return_type = get_type<R>();
    signature.arguments_types.resize(sizeof...(Args));
    build_signature<Args...>(signature,0);
    return signature;
}


template <typename Sig>
struct signature_builder_t;


template <typename R,typename ... Args>
struct signature_builder_t<R(Args...)>
{
    typedef R(*FunctionType)(Args...);
    static function_signature_t & value(){
        static function_signature_t signature = build_signature<R,Args...>();
        return signature;
    }
};

}





template <typename R,typename ...Args>
inline function_signature_t create_function_signature(){
    return detail::build_signature<R,Args...>();
}

struct parameter_t {
    std::string name;
    gccjit::param value;

    parameter_t(){}
    parameter_t(const std::string& n,const gccjit::param& v):
        name(n),
        value(reinterpret_cast<gcc_jit_param *> (v.get_inner_lvalue())){}
};


struct function_t {

    function_t(){}
    operator bool()const{return m_function.get_inner_function();}

    std::string mangled_name()const;

    gccjit::function& jit_function(){return m_function;}

    bool matches(const function_signature_t &,const std::string& name)const;

    template <typename T>
    rvalue_t create_variable(const std::string & name);
    boost::optional<parameter_t> find_paramerter(const std::string & name);

private:
    std::string m_name;
    function_signature_t m_signature;
    gccjit::function m_function;
    std::vector<parameter_t> m_parameters;    
    friend class jit_code_t;

};

template <typename T>
rvalue_t function_t::create_variable(const std::string & name)
{
    return m_function.new_local(get_jit_type<T>(m_function.get_context()),name);
}

struct jit_code_t : boost::noncopyable {

    jit_code_t();
    ~jit_code_t();

    context_t & context(){return m_jit_context;}
    const context_t & context()const{return m_jit_context;}

    template <typename R>
    function_t create_expression_function();

    function_t create_function(const ast::function_declaration_t &);

    template <typename T>
    rvalue_t create_constant(T value);

    boost::optional<function_t> find_function(const function_signature_t&,const std::string & name)const;

    void compile();

    void* get_compiled_function(const std::string & mangled_name)const;

    template <typename Sig>
    std::function<Sig> get_function(const std::string& name);


public:
    static std::string create_mangled_name(const std::string & name,const function_signature_t &);
    static const char* type_to_string(jit_type_e t);

private:
    context_t m_jit_context;
    std::vector<function_t> m_functions;
    gcc_jit_result * m_compile_result = nullptr;

};


template <typename Sig>
std::function<Sig> jit_code_t::get_function(const std::string &name)
{
    function_signature_t signature = detail::signature_builder_t<Sig>::value();
    void * f = get_compiled_function(create_mangled_name(name,signature));
    if(f) return (typename detail::signature_builder_t<Sig>::FunctionType)f;
    throw std::runtime_error("failed to find function "+name);
}

template <typename R>
function_t jit_code_t::create_expression_function()
{
    static int index = 0;
    function_t f;
    f.m_name = "expression_function_"+std::to_string(index++);
    f.m_signature.return_type = get_type<double>();

    static std::vector<gccjit::param> params;
    auto return_type = get_jit_type<R>(m_jit_context);

    gccjit::function func = m_jit_context.new_function (GCC_JIT_FUNCTION_EXPORTED,
                                                        return_type,
                                                        f.m_name,
                                                        params,
                                                        false);
    f.m_function = func;
    m_functions.push_back(f);
    return f;
}

template<typename T>
rvalue_t jit_code_t::create_constant(T value)
{
    return m_jit_context.new_rvalue(get_jit_type<T>(m_jit_context),value);
}




}} // ev::vm


#endif // EV_JIT_TYPES_H
