#ifndef EV_VM_JIT_CONTEXT_H
#define EV_VM_JIT_CONTEXT_H

#include <memory>

#include <boost/noncopyable.hpp>

#include "data_fwd_declare.h"
#include "utils.h"
#include "object.h"
#include "value.h"

namespace ev { namespace vm { namespace jit {

struct type_t
{

    basic_type_kind_e kind()const{return m_kind;}
    operator type_data_t()const {return m_data;}
    operator bool ()const {return m_data;}

protected:
    basic_type_kind_e m_kind;
    type_data_t m_data = nullptr;
    friend class context_t;
};



class context_private_t;
class function_t;
class value_t;
class module_t;
class compilation_scope_t;
class function_id_t;

struct context_t : boost::noncopyable {

    context_t();
    ~context_t();

    template <typename T>
    value_t new_constant(T value);

    type_t get_type(basic_type_kind_e kind);

    module_t new_module(const std::string & name);
    module_t find_module(const std::string & name)const;
    module_t main_module()const;

    value_t new_call(const function_t &, const std::vector<value_data_t> &args);

    function_t find_function(const function_id_t & id,const std::string & module = "main")const;

    void compile();


protected:
    value_t new_constant(basic_type_kind_e kind,void * val);
private:


    context_private_t* d;

    friend class function_t;
    friend class value_t;
    friend class block_t;

};

template <typename T>
value_t context_t::new_constant(T value)
{
    return new_constant(get_basic_type_kind<T>(),&value);
}






}}}



#endif//EV_VM_JIT_CONTEXT_H
