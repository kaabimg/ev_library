#pragma once

#include "runtime_function.hpp"
#include "jit/function_signature.hpp"

#include <ev/core/preprocessor.hpp>

#include <string>
#include <functional>

namespace ev
{
namespace vm
{
struct virtual_machine
{
    virtual_machine();
    ~virtual_machine();
    void eval(const std::string&);

    template <typename Sig>
    runtime_function<Sig> build(const std::string&);

protected:
    void* create_function(const std::string& str,
                          const jit::function_signature& expected_signature);

private:
    EV_PRIVATE(virtual_machine)
};

template <typename Sig>
runtime_function<Sig> virtual_machine::build(const std::string& line)
{
    return runtime_function<Sig>{
        create_function(line, jit::create_function_signature<Sig>())};
}
}
}
