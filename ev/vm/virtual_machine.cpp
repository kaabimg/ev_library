#include "virtual_machine.h"

#include "compiler.h"
#include "jit_types.h"
#include "parser.h"
#include "ast.h"


#include <iostream>
#include <memory>
#include <unordered_set>

using namespace ev::vm;


namespace ev { namespace vm {


struct virtual_machine_t::data_t {

    compiler_t compiler;
    parser_t parser;
    jit_code_t code;

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
        ev::debug() << "Syntax error :"<<result.error_string;
        return;
    }

    function_t function = d->compiler.compile(*result.statement.get(),d->code);

    if(function){
        d->code.compile();
    }


    if(function && result.statement->type() == ast::statement_type_e::expression){
//        ev::info() << ;
    }

}


void* virtual_machine_t::create_function(
        const std::string& str,
        const function_signature_t & expected_signature)
{
    parser_result_t result = d->parser.parse(str);

    if(!result.success){
        throw std::runtime_error ("Syntax error : "+result.error_string);
    }

    if(result.statement->type()!=ast::statement_type_e::function_declaration){
        throw std::runtime_error ("Expected a function definition");
    }



    function_t function = d->compiler.compile(*result.statement.get(),d->code);
    if(!function){
        throw std::runtime_error ("Failed to compile expresion");
    }

    if(function.matches(
                expected_signature,
                result.statement.get()->as<ast::function_declaration_t>().name.value))
    {
        d->code.compile();
        return d->code.get_compiled_function(function.mangled_name());
    }

    throw std::runtime_error("Expexted signature not matching compiled function");


}
