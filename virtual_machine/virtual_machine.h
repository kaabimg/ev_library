#ifndef EV_VIRTUAL_MACHINE_H
#define EV_VIRTUAL_MACHINE_H


#include <string>
#include <core/preprocessor.h>
#include <functional>
#include "jit_types.h"


namespace ev { namespace vm {


struct virtual_machine_t {

    virtual_machine_t();
    ~virtual_machine_t();
    void eval(const std::string&);

    template<typename R,typename ... Arg>
    std::function<R(Arg...)> build(const std::string &);



protected:
    void * create_function(const std::string & str, const function_signature_t& expected_signature);


private:
    EV_PRIVATE(virtual_machine_t)


};


template<typename R,typename ... Arg>
std::function<R(Arg...)> virtual_machine_t::build(const std::string & line){

    typedef R (*callable_t)(Arg...);

    void * func = create_function(line,create_function_signature<R,Arg...>());

    if(func){
        return (callable_t)func;
    }
    return [](Arg...)->R {return R();};

}

}}





#endif // EV_VIRTUAL_MACHINE_H
