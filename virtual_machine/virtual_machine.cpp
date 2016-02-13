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
    context_t context;

    std::list<vm_object_t> objects;
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
        core::debug() << "Syntax error : "<<result.error_string;
        return;
    }

    function_t function = d->compiler.compile(*result.statement.get(),d->context);


    if(function && result.statement->type() == ast::statement_type_e::expression){
        core::debug() << function.call<double>();
    }
    else if(result.statement->type() == ast::statement_type_e::variable_declaration){
        const double * value =
                d->context.global_scope().find_host_variable(
                    result.statement->as<ast::variable_declaration_t>().variable_name.value
                    );
        if(value){
            core::debug()<<*value;
        }

    }

}


void* virtual_machine_t::create_function(
        const std::string& str,
        const function_signature_t & expected_signature)
{
    parser_result_t result = d->parser.parse(str);

    if(!result.success){
        core::debug() << "Syntax error : "<<result.error_string;
        return nullptr;
    }

    if(result.statement->type()!=ast::statement_type_e::function_declaration){
        core::debug() << "Error : expected a function definition";
    }



    function_t function = d->compiler.compile(*result.statement.get(),d->context);
    if(!function){
        return nullptr;
    }

    if(function.matches(expected_signature)){
        return function.to_closure();
    }
    return nullptr;

}
