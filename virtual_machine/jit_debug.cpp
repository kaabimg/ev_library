#include "jit_debug.h"
#include "jit_types.h"
#include <core/logging.h>
#include <jit/jit-dump.h>
#include <fstream>

using namespace ev::vm;

void debug::dump(const function_t& function)
{
    core::debug()<<"-------------- function -------------------";
    jit_dump_function(stdout ,function.data(),function.name());
    core::debug()<<"";
}

void debug::dump(const value_t& value,const std::string & name)
{
    core::debug()<<"-------------- value" <<name<< "-------------------";
    jit_dump_value(
                stdout ,
                value.function().data(),
                value.data(),
                nullptr
                );
    core::debug()<<"";
}
