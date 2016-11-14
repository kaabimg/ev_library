#ifndef EV_VIRTUAL_MACHINE_H
#define EV_VIRTUAL_MACHINE_H


#include <string>
#include <functional>

#include <ev/core/preprocessor.h>
#include "jit/utils.h"


namespace ev { namespace vm {


struct virtual_machine_t {

    virtual_machine_t();
    ~virtual_machine_t();
    void eval(const std::string&);

    template<typename Sig>
    std::function<Sig> build(const std::string &);



protected:
    void * create_function(const std::string & str, const jit::function_signature_t& expected_signature);


private:
    EV_PRIVATE(virtual_machine_t)


};


template<typename Sig>
std::function<Sig> virtual_machine_t::build(const std::string & line)
{
    void * func = create_function(line,jit::create_function_signature<Sig>());
    return (typename jit::signature_builder_t<Sig>::FunctionType)func;
}

}}





#endif // EV_VIRTUAL_MACHINE_H
