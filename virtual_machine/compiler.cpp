#include "compiler.h"

#include <iostream>
#include <sstream>
#include <core/logging.h>

#include "jit_debug.h"

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
    compiler_step_t(context_t & context_):context(context_){}

    int level = 0;


    context_t & context;


};


struct jit_compiler_t : compiler_step_t {

    jit_compiler_t(context_t & context_):compiler_step_t(context_){}

    value_t build(double v){

        LOG_FUNCTION(v);
        return context.create_constant(v);
    }

    value_t find_variable(const ast::identifier_t & var)
    {
        LOG_FUNCTION(var.value);

        value_t v = context.current_scope().find_variable(var.value);

        if(!v){
            const double * hv = context.global_scope().find_host_variable(var.value);

            v = jit_value_create_nint_constant(
                        context.current_function().data(),
                        jit_type_void_ptr,
                        (long int)hv
                        );

            v = jit_insn_load_elem(
                        context.current_function().data(),
                        v.data(),
                        jit_value_create_nint_constant(
                                        context.current_function().data(),
                                        get_builtin_type<int>(),
                                        0
                                        ),
                        jit_type_sys_double
                        );


        }

        if(!v){
            THROW("variable "+var.value+" not found",var);

        }


        return v;
    }

    value_t build(const ast::expression_t & expression)
    {
        LOG_FUNCTION("");


        try {

            value_t lhs = build(expression.first);

            if(expression.rest.empty()){
                return lhs;
            }

            for(const ast::operation_t & operation : expression.rest){
                value_t rhs = build(operation.operand);

                switch (operation.op) {
                case ast::operator_type_e::plus:
                    lhs = context.current_function().add(lhs,rhs);
                    break;
                case ast::operator_type_e::minus:
                    lhs = context.current_function().sub(lhs,rhs);

                    break;
                case ast::operator_type_e::times:
                    lhs = context.current_function().mul(lhs,rhs);

                    break;
                case ast::operator_type_e::divide:
                    lhs = context.current_function().div(lhs,rhs);
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

    value_t build(const ast::object_t & obj)
    {
        LOG_FUNCTION("");
        try {
            std::vector<value_t> values(obj.members.size());

            for (size_t i = 0; i < values.size(); ++i) {
                values[i] = build(obj.members[i].value);
            }

        }
        catch(...){
            throw;
        }
    }

    value_t build(const ast::array_t & array)
    {
        LOG_FUNCTION("");
        try {
            std::vector<value_t> values(array.values.size());

            for (size_t i = 0; i < values.size(); ++i) {
                values[i] = build(array.values[i]);
            }

        }
        catch(...){
            throw;
        }
    }

    value_t build(const ast::operand_t & operand)
    {
        LOG_FUNCTION("");


        try {
            switch (operand.type()) {
            case ast::operand_type_e::number:
                return build(operand.as<double>());
            case ast::operand_type_e::variable:
                return find_variable(operand.as<ast::variable_t>());
            case ast::operand_type_e::unary_expression:
                return build(operand.as<x3::forward_ast<ast::unary_t>>().get());
            case ast::operand_type_e::expression:
                return build(operand.as<x3::forward_ast<ast::expression_t>>().get());
            case ast::operand_type_e::function_call:
                return build(operand.as<x3::forward_ast<ast::function_call_t>>().get());
            case ast::operand_type_e::array:
                return build(operand.as<x3::forward_ast<ast::array_t>>().get());
            case ast::operand_type_e::object:
                return build(operand.as<x3::forward_ast<ast::object_t>>().get());
            default:
                assert(false && "unknown operand");
                return value_t();
            }
        }
        catch(...){
            throw;
        }
    }



    value_t build(const ast::unary_t & expression)
    {
        LOG_FUNCTION("");
        try {

            value_t rhs = build(expression.operand);
            switch (expression.op) {
            case ast::operator_type_e::positive:
                return rhs;
            case ast::operator_type_e::negative:
                return context.current_function().negative(rhs);
            default:{
                assert(false && "Unknown unary oprator");
                return value_t();
            }
            }
        }
        catch(...){
            throw;
        }
    }

    value_t build(const ast::function_call_t & func_call)
    {
        LOG_FUNCTION(func_call.name.value);

        try {

            function_signature_t signature;
            signature.return_type = get_builtin_type<double>();
            signature.arguments_types = std::vector<jit_type_t>(
                        func_call.arguments.size(),
                        get_builtin_type<double>()
                        );

            function_t function = context.current_scope().find_function(func_call.name.value,signature);

            if(!function){
                THROW("fundtion "+func_call.name.value+" not found",func_call);
            }

            std::vector<jit_value_t> args (func_call.arguments.size());

            size_t i = 0;
            for(const ast::expression_t& expression : func_call.arguments){
                args[i++] = build(expression).data();
            }


            return context.current_function().create_function_call(function,args);

        }
        catch(...){
            throw;
        }

    }




    void build(const ast::assignement_t & assignement)
    {

        LOG_FUNCTION(assignement.variable_name.value);


        try {

            value_t variable = find_variable(assignement.variable_name);
            value_t expression = build(assignement.expression);
            variable.store(expression);
        }
        catch(...){
            throw;
        }
    }


    void build(const ast::variable_declaration_t & var_dec)
    {

        LOG_FUNCTION(var_dec.variable_name.value);

        try {

            if(context.global_scope().find_host_variable(var_dec.variable_name.value)){
                THROW("variable "+var_dec.variable_name.value+" already exist",
                      var_dec);
            }

            function_t declare =  create_top_level_expression_function(var_dec.expression);

            double value = declare.call<double>();

            context.global_scope().create_host_variable(
                        var_dec.variable_name.value,
                        value
                        );

        }
        catch(...){
            throw;
        }
    }




    function_t build(const ast::function_declaration_t & function_dec)
    {
        LOG_FUNCTION(function_dec.name.value);

        function_t function = context.create_function(function_dec);
        context.push(function);
        value_t expression = build(function_dec.expression);
        context.pop();
        context.current_scope().register_function(function);

        function.set_return(expression);
        function.compile();
        return function;

    }

    function_t create_top_level_expression_function(const ast::expression_t & expression)
    {
        function_t function = context.create_expression_function();
        context.push(function);
        value_t exp_value = build(expression);
        context.pop();
        function.set_return(exp_value);
        function.compile();
        return function;
    }


};

}}}


function_t compiler_t::compile(const ast::statement_t & statement, context_t& context)
{
    compiler::jit_compiler_t compiler(context);

    function_t function;
    try {

        switch (statement.type()) {
        case ast::statement_type_e::expression:{
            function = compiler.create_top_level_expression_function(statement.as<ast::expression_t>());
            break;
        }
        case ast::statement_type_e::function_declaration:{
            function = compiler.build(statement.as<ast::function_declaration_t>());
            break;
        }
        case ast::statement_type_e::variable_declaration:{
            compiler.build(statement.as<ast::variable_declaration_t>());
            break;
        }
        default:
            break;
        }


    }
    catch(const compiler::compile_error_t & error){
        ev::debug() << "Compile error :"<<error.what();
        return function_t();

    }


    //    debug::dump(context.main_function());
    //    if(function){
    //        debug::dump(function);
    //    }


    return function;
}

