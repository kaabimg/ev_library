#ifndef EV_JIT_TYPES_H
#define EV_JIT_TYPES_H

#include <core/preprocessor.h>
#include <core/logging.h>
#include <jit/jit.h>

#include "ast.h"

#include <unordered_map>
#include <vector>
#include <list>


namespace ev { namespace vm {



namespace details {

template <typename T>
inline jit_type_t get_builtin_type_impl(){
    assert(false && "unmatched type");
    return nullptr;
}

template<>
inline jit_type_t get_builtin_type_impl<void>(){
    return jit_type_void;
}


template<>
inline jit_type_t get_builtin_type_impl<bool>(){
    return jit_type_sys_bool;
}

template<>
inline jit_type_t get_builtin_type_impl<int>(){
    return jit_type_sys_int;
}

template<>
inline jit_type_t get_builtin_type_impl<uint>(){
    return jit_type_sys_uint;
}

template<>
inline jit_type_t get_builtin_type_impl<char>(){
    return jit_type_sys_char;
}

template<>
inline jit_type_t get_builtin_type_impl<float>(){
    return jit_type_sys_float;
}

template<>
inline jit_type_t get_builtin_type_impl<double>(){
    return jit_type_sys_double;
}
}


template <typename T>
inline jit_type_t get_builtin_type()
{
    return details::get_builtin_type_impl<
                typename std::remove_const<
                    typename std::remove_reference<T>::type
                >::type
            >();
}

struct context_t;
struct function_t;


#define EV_JIT_OBJECT(name) \
    name##_t():m_jit_##name(nullptr){}\
    name##_t(jit_##name##_t d):m_jit_##name(d){}\
    name##_t(const name##_t & other):m_jit_##name(other.m_jit_##name){}\
    name##_t(name##_t && other):m_jit_##name(other.m_jit_##name){}\
    name##_t & operator=(const name##_t & other){m_jit_##name = other.m_jit_##name; return *this;}\
    name##_t & operator=(jit_##name##_t d){m_jit_##name = d; return *this;}\
    bool is_valid()const{return m_jit_##name;}\
    operator bool ()const{return m_jit_##name;}\
    jit_##name##_t data()const{return m_jit_##name;}


struct value_t  {
    EV_JIT_OBJECT(value)

    jit_type_t type()const;

    function_t function()const;

    value_t load()const;
    value_t load_relative()const;

    void store(const value_t& v);
    void store_relative(const value_t& v);

private:
    jit_value_t m_jit_value;
    friend class function_t;
};


struct block_t  {
    EV_JIT_OBJECT(block)

private:

    jit_block_t m_jit_block;
};



struct function_signature_t {
    jit_type_t return_type;
    std::vector<jit_type_t> arguments_types;

    bool operator ==(const function_signature_t & another)const {
        return return_type == another.return_type && arguments_types == another.arguments_types;
    }
};



struct function_meta_data_t {
    std::string name;
    std::vector<std::string> arguments_names;
    function_signature_t signature;

};


enum class instruction_type_e {
    add,
    sub,
    mul,
    div,
    function_call,
    functtion_return
};


struct function_t {

    EV_JIT_OBJECT(function)

    const char * name()const;
    const function_meta_data_t & meta_data()const;

    size_t args_count()const;
    bool matches(const std::string & name,const function_signature_t & signature)const;
    bool matches(const function_signature_t & signature)const;

    value_t get_parmeter(const std::string & name) const;


    // variables && constants

    value_t create_variable();
    value_t create_constant(double val);


    // instructions

    value_t add(const value_t & v1,const value_t & v2);
    value_t sub(const value_t & v1,const value_t & v2);
    value_t mul(const value_t & v1,const value_t & v2);
    value_t div(const value_t & v1,const value_t & v2);
    value_t negative(const value_t & v);
    void store(const value_t & dest,const value_t & source);

    value_t create_function_call(const function_t & func, const std::vector<jit_value_t>& args);
    void set_return(const value_t & v);


    // compilation
    void abandon();
    bool compile();
    bool is_compiled()const;
    void set_recompilable(bool);
    bool is_recopmilable()const;




    // apply    

    void * to_closure();

    template <typename R,typename ...Arg>
    R call(Arg... args);

    template <typename R>
    R call();


    template <typename R,typename ...Arg>
    R unsafe_call(Arg... args);

    template <typename R>
    R unsafe_call();




protected:
    static void clear_meta_data(void*);
    void set_meta_data(function_meta_data_t*);


private:
    friend class context_t;

    jit_function_t m_jit_function;

};

enum class scope_type_e {
    function,
    block
};

struct scope_t {

    scope_t* parent_scope = nullptr;
    std::vector<scope_t> sub_scopes;


    struct data_t{
        function_t function;
        block_t block;
        scope_type_e type;
        std::vector<function_t> functions;
        std::unordered_map<std::string,value_t> variables;
        std::unordered_map<std::string,double> host_variables;
    } data;

/////

    scope_t(const function_t & func,scope_t * parent){
        data.function  = func;
        data.type = scope_type_e::function;
        parent_scope = parent;
    }

    scope_t(const block_t & block,scope_t * parent){
        data.block  = block;
        data.type = scope_type_e::block;
        parent_scope = parent;
    }


    function_t enclosing_function()const;


    bool is_function()const{return data.type == scope_type_e::function;}
    bool is_block()const{return data.type == scope_type_e::block;}


    void register_function(const function_t & );
    void register_variable(const std::string & name,const value_t & v);

    value_t find_variable(const std::string & name)const;
    function_t find_function(const std::string & name,const function_signature_t& )const;


    value_t create_variable(const std::string & name,double value);


    double * create_host_variable(const std::string & name,double value);
    const double* find_host_variable(const std::string & name)const;

};



struct context_t :  boost::noncopyable {

    context_t();
    ~context_t();


    function_t create_function(const ast::function_declaration_t & func_decl);
    function_t create_expression_function();
    function_t create_variable_declaration_function();


    void push(const function_t & f);
    void push(const block_t & f);
    void pop();

    function_t current_function()const;
    function_t main_function()const;

    scope_t & current_scope();
    scope_t & global_scope();

    value_t create_constant(double value);


protected:
    void create_main_function();

    static void handle_exception(int type);
private:
    jit_context_t m_jit_contex;
    std::unique_ptr<scope_t> m_main_scope;
    scope_t * m_current_scope = nullptr;

};

inline void build_signature(function_signature_t &,size_t){}

template <typename H>
inline void build_signature(function_signature_t & signature,size_t pos){
    signature.arguments_types[pos] = get_builtin_type<H>();
}
template <typename H,typename H2,typename ...T>
inline void build_signature(function_signature_t & signature,size_t pos){
    signature.arguments_types[pos] = get_builtin_type<H>();
    build_signature<H2,T...>(signature,++pos);
}


template <typename R,typename ...Arg>
inline function_signature_t create_function_signature(){
    function_signature_t signature;
    signature.return_type = get_builtin_type<R>();
    signature.arguments_types.resize(sizeof...(Arg));
    build_signature<Arg...>(signature,0);
    return std::move(signature);
}

template <typename R,typename ...Arg>
R function_t::call(Arg... args){

    if(meta_data().signature.return_type != get_builtin_type<R>()){
        ev::debug()<<"Error : return value type mismatch";
        return R();
    }

    size_t args_count = sizeof...(args);

    if(args_count != this->args_count()){
        ev::debug()<<"Error : function"<<name()<<"accepts"<<this->args_count()<<"arguments";
        return R();
    }

    function_signature_t signature = create_function_signature<R,Arg...>();

    if(matches(signature)){
        typedef R (*callable_t)(Arg...);
        callable_t callable = (callable_t)jit_function_to_closure(m_jit_function);
        return callable(args...);
    }
    else {
        ev::debug()<<"Error : function signature mismatch";
        return R();

    }

}

template <typename R>
R function_t::call(){

    if(meta_data().signature.return_type != get_builtin_type<R>()){
        ev::debug()<<"Error : return value type mismatch";
        return R();
    }

    if(!args_count() ){
        return unsafe_call<R>();
    }
    else {
        ev::debug()<<"Error : functions"<<name()<<"accepts"<<args_count()<<"arguments";
        return R();

    }
}



template <typename R,typename ...Arg>
R function_t::unsafe_call(Arg... args){

    typedef R (*callable_t)(Arg...);
    callable_t callable = (callable_t)jit_function_to_closure(m_jit_function);
    return callable(args...);

}

template <typename R>
R function_t::unsafe_call(){

    typedef R (*callable_t)();
    callable_t callable = (callable_t)jit_function_to_closure(m_jit_function);
    return callable();
}




}}


#endif // EV_JIT_TYPES_H
