#include "compiler.h"

#include <iostream>
#include <sstream>
#include <ev/core/logging.h>

#include "ast.h"
#include "jit/context.h"
#include "jit/function.h"
#include "jit/module.h"

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
    compiler_step_t(jit::context_t & _context):context(_context){}
    int level = 0;
    jit::context_t & context;
};


struct jit_compiler_t : compiler_step_t {

    jit_compiler_t(jit::context_t & context):compiler_step_t(context){}

    jit::value_t build(double v){

        LOG_FUNCTION(v);
        return context.new_constant<double>(v);
    }


    jit::value_t build(const ast::expression_t & expression)
    {
        LOG_FUNCTION("");

        try {

            jit::value_t lhs = build(expression.first);

            if(expression.rest.empty()){
                return lhs;
            }

            for(const ast::operation_t & operation : expression.rest){
                jit::value_t rhs = build(operation.operand);

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



    jit::value_t build(const ast::operand_t & operand)
    {
        LOG_FUNCTION("");


        try {
            switch (operand.type()) {
            case ast::operand_type_e::number:
                return build(operand.as<double>());

            case ast::operand_type_e::variable:{
                jit::value_t var = context.main_module().find_variable(
                            operand.as<ast::variable_t>().value
                            );
                if(var)return var;
                THROW("Variable "+operand.as<ast::variable_t>().value+" not found",operand.as<ast::variable_t>());
            }

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



    jit::value_t build(const ast::unary_t & expression)
    {
        LOG_FUNCTION("");
        try {

            jit::value_t rhs = build(expression.operand);
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

    jit::value_t build(const ast::function_call_t & func_call)
    {
        LOG_FUNCTION(func_call.name.value);

        try {

            jit::function_id_t signature;
            signature.return_type = jit::get_basic_type_kind<double>();
            signature.args_type = std::vector<jit::basic_type_kind_e>(
                        func_call.arguments.size(),
                        jit::get_basic_type_kind<double>()
                        );

            signature.name = func_call.name.value;

            jit::function_t called_fun = context.find_function(signature);
            if(!called_fun){
                THROW("function "+func_call.name.value+" not found",func_call);
            }

            std::vector<jit::value_data_t> args (func_call.arguments.size());

            size_t i = 0;
            for(const ast::expression_t& expression : func_call.arguments){
                args[i++] = build(expression);
            }

            return context.new_call(called_fun,args);

        }
        catch(...){
            throw;
        }

    }


    jit::function_t build(const ast::function_declaration_t & function_dec)
    {
        LOG_FUNCTION(function_dec.name.value);
        jit::function_creation_info_t info;
        info.name = function_dec.name.value;
        info.return_type = jit::get_basic_type_kind<double>();
        info.args_type.resize(function_dec.arguments.size());
        std::fill(info.args_type.begin(),info.args_type.end(),
                  info.return_type);



        info.args_names.resize(function_dec.arguments.size());
        int i = 0;
        for (auto & name : function_dec.arguments){
            info.args_names[i++] = name.value;
        }

        jit::function_t function = context.main_module().new_function(info);

        context.main_module().push_scope(function);

        jit::block_t main_block = function.new_block("entry");

        context.main_module().push_scope(main_block);

        main_block.set_as_insert_point();
        main_block.set_return(build(function_dec.expression));


        context.main_module().pop_scope();
        context.main_module().pop_scope();


        if(!function.finalize()){
            ev::error() << "failed to compile function";
        }

        return function;

    }

    jit::function_t create_top_level_expression_function(const ast::expression_t & expression)
    {
        //TODO
        jit::function_creation_info_t info;
        jit::function_t function = context.main_module().new_function(info);
        return function;
    }


};

}}}


jit::function_t compiler_t::compile(const ast::statement_t &statement, jit::context_t& context)
{
    compiler::jit_compiler_t compiler(context);

    try {

        switch (statement.type()) {
        case ast::statement_type_e::expression:{
            return compiler.create_top_level_expression_function(statement.as<ast::expression_t>());
            break;
        }
        case ast::statement_type_e::function_declaration:{
            return compiler.build(statement.as<ast::function_declaration_t>());
            break;
        }

        default:
            break;
        }


    }
    catch(const compiler::compile_error_t & error){
        throw std::runtime_error(error.what());
    }


    return jit::function_t();
}

