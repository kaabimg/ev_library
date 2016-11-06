#include "compiler.h"

#include <iostream>
#include <sstream>
#include <ev/core/logging.h>


using namespace ev::vm;

namespace x3 = boost::spirit::x3; 

//#define LOG_FUNCTION(...) level_counter c(level);EV_TRACE(level) << __VA_ARGS__
//#define LOG_FUNCTION level_counter c(level);std::cout<< create_indent(level) << __PRETTY_FUNCTION__ << std::endl
#define LOG_FUNCTION(...)


#define COMPILER_ASSERT(condition,error_str,location) \
    if(!condition) return set_error(error_str,location)


#define ERROR_CHECKPOINT if(has_error)return nullptr

#define THROW(error_str,location) throw compile_error_t(error_str,location)

namespace ev  { namespace vm { namespace compiler {




struct level_counter {
    level_counter(int & l):level(l){++level;}
    ~level_counter(){--level;}
    int & level ;
};




struct compile_error_t : std::exception {


    compile_error_t(){}

    compile_error_t(const std::string & msg,const x3::position_tagged & location) noexcept
        : m_message(msg),m_location(location){}
    virtual const char* what() const noexcept {return m_message.c_str();}
    x3::position_tagged where()const noexcept {return m_location;}

private:
    std::string m_message;
    x3::position_tagged m_location;

};


struct compiler_step_t {
    compiler_step_t(jit_code_t & _code):code(_code){}

    int level = 0;


    jit_code_t & code;


};


struct jit_compiler_t : compiler_step_t {

    jit_compiler_t(jit_code_t & context_):compiler_step_t(context_){}

    rvalue_t build(double v){

        LOG_FUNCTION(v);
        return code.create_constant<double>(v);
    }


    rvalue_t build(const ast::expression_t & expression)
    {
        LOG_FUNCTION("");

        try {

            rvalue_t lhs = build(expression.first);

            if(expression.rest.empty()){
                return lhs;
            }

            for(const ast::operation_t & operation : expression.rest){
                rvalue_t rhs = build(operation.operand);

                switch (operation.op) {
                case ast::operator_type_e::plus:
                    lhs = lhs + rhs;
                    break;
                case ast::operator_type_e::minus:
                    lhs = lhs -rhs;

                    break;
                case ast::operator_type_e::times:
                    lhs = lhs * rhs;

                    break;
                case ast::operator_type_e::divide:
                    lhs = lhs/rhs;
                    break;
                default:
                    break;
                }
            }
            return lhs;
        }
        catch(...){
            throw;
        }


    }



    rvalue_t build(const ast::operand_t & operand)
    {
        LOG_FUNCTION("");


        try {
            switch (operand.type()) {
            case ast::operand_type_e::number:
                return build(operand.as<double>());

                //TODO
//            case ast::operand_type_e::variable:
//                return find_variable(operand.as<ast::variable_t>());

            case ast::operand_type_e::unary_expression:
                return build(operand.as<x3::forward_ast<ast::unary_t>>().get());

            case ast::operand_type_e::expression:
                return build(operand.as<x3::forward_ast<ast::expression_t>>().get());

            case ast::operand_type_e::function_call:
                return build(operand.as<x3::forward_ast<ast::function_call_t>>().get());

            default:
                THROW("unknown operand",x3::position_tagged());

            }
        }
        catch(...){
            throw;
        }
    }



    rvalue_t build(const ast::unary_t & expression)
    {
        LOG_FUNCTION("");
        try {

            rvalue_t rhs = build(expression.operand);
            switch (expression.op) {
            case ast::operator_type_e::positive:
                return rhs;
            case ast::operator_type_e::negative:
                return -rhs;
            default:
                THROW("Unknown unary oprator",expression);
            }
        }
        catch(...){
            throw;
        }
    }

    rvalue_t build(const ast::function_call_t & func_call)
    {
        LOG_FUNCTION(func_call.name.value);

        try {

            function_signature_t signature;
            signature.return_type = get_type<double>();
            signature.arguments_types = std::vector<jit_type_e>(
                        func_call.arguments.size(),
                        get_type<double>()
                        );

            boost::optional<function_t> function = code.find_function(signature,func_call.name.value);
            if(function){
            }
            else {
                THROW("function "+func_call.name.value+" not found",func_call);
            }

            std::vector<rvalue_t> args (func_call.arguments.size());

            size_t i = 0;
            for(const ast::expression_t& expression : func_call.arguments){
                args[i++] = build(expression);
            }


            return code.context().new_call(function.get().jit_function(),args);

        }
        catch(...){
            throw;
        }

    }


    function_t build(const ast::function_declaration_t & function_dec)
    {
        LOG_FUNCTION(function_dec.name.value);

        function_t function = code.create_function(function_dec);
        block_t f_block = function.jit_function().new_block();
        f_block.end_with_return(build(function_dec.expression));
        return function;

    }

    function_t create_top_level_expression_function(const ast::expression_t & expression)
    {
        function_t function = code.create_expression_function<double>();
        block_t f_block = function.jit_function().new_block();
        f_block.end_with_return(build(expression));
        return function;
    }


};

}}}


function_t compiler_t::compile(const ast::statement_t &statement, jit_code_t& code)
{
    compiler::jit_compiler_t compiler(code);

    try {

        switch (statement.type()) {
        case ast::statement_type_e::expression:{
            return compiler.create_top_level_expression_function(statement.as<ast::expression_t>());
            break;
        }
        case ast::statement_type_e::function_declaration:{
            return  compiler.build(statement.as<ast::function_declaration_t>());
            break;
        }

        default:
            break;
        }


    }
    catch(const compiler::compile_error_t & error){
        ev::debug() << "Compile error :"<<error.what();
    }


    return function_t();
}

