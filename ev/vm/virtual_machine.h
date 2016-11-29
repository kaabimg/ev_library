#pragma once


#include <string>
#include <functional>

#include <ev/core/preprocessor.h>

#include "runtime_function.h"
#include "jit/function_signature.h"


namespace ev { namespace vm {


struct virtual_machine_t {

    virtual_machine_t();
    ~virtual_machine_t();
    void eval(const std::string&);

    template<typename Sig>
    runtime_function_t<Sig> build(const std::string &);

protected:
    void *create_function(const std::string & str, const jit::function_signature_t& expected_signature);

private:
    EV_PRIVATE(virtual_machine_t)
};


template<typename Sig>
runtime_function_t<Sig> virtual_machine_t::build(const std::string & line)
{
    return runtime_function_t<Sig>{
        create_function(
                    line,
                    jit::create_function_signature<Sig>())
    };
}

}}

