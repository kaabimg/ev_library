#include "virtual_machine.h"

#include "parser/ast.h"
#include "parser/parser.h"
#include "compiler/compiler.h"
#include "jit/context.h"
#include "jit/function.h"
#include "vm_exceptions.h"

#include <iostream>
#include <memory>
#include <unordered_set>

using namespace ev::vm;


namespace ev { namespace vm {


struct virtual_machine_t::data_t
{
    parser_t parser;
    jit::context_t context;
    compiler_t compiler {context};

};

}}

virtual_machine_t::virtual_machine_t():d(new virtual_machine_t::data_t)
{
}

virtual_machine_t::~virtual_machine_t()
{
    delete d;
}

void virtual_machine_t::eval(const std::string& line)
{
    if(line.size() && *line.begin() == '#'){
        return ;
    }

    parser_result_t result = d->parser.parse(line);

    if(!result.success){
        throw syntax_error_t("Syntax error :"+result.error_string);
        return;
    }

    jit::function_t function = d->compiler.compile(*result.statement.get());

    if(function){
        d->context.compile();
    }

    //TODO
    return;


    if(function && result.statement->type() == ast::statement_type_e::expression){
//        ev::info() << ;
    }

}

void* virtual_machine_t::create_function(
        const std::string& str,
        const jit::function_signature_t & expected_signature)
{
    parser_result_t result = d->parser.parse(str);

    if(!result.success){
        throw syntax_error_t ("Syntax error : "+result.error_string);
    }

    if(result.statement->type() != ast::statement_type_e::function_declaration){
        throw syntax_error_t ("Expected a function definition");
    }

    jit::function_t function = d->compiler.compile(*result.statement.get());
    if(!function){
        throw compile_error_t("Failed to compile expresion");
    }

    if(reinterpret_cast<const jit::function_signature_t&>(function.info()) == expected_signature)
    {
        d->context.compile();
        //TODO
        return nullptr;
    }

    throw compile_error_t("Expexted signature not matching compiled function");

}
