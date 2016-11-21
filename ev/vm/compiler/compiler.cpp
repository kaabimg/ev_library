#include "compiler.h"

#include <iostream>
#include <sstream>

#include <ev/core/logging.h>
#include <ev/core/scope_exit.h>

#include "../parser/ast.h"
#include "../jit/context.h"
#include "../jit/function.h"
#include "../jit/module.h"

#include "../vm_exceptions.h"

using namespace ev::vm;

namespace x3 = boost::spirit::x3; 

jit::function_t compiler_t::compile(const ast::statement_t &statement)
{

    switch (statement.type()) {
    case ast::statement_type_e::expression:{
        return create_top_level_expression_function(statement.as<ast::expression_t>());
        break;
    }
    case ast::statement_type_e::function_declaration:{
        return build(statement.as<ast::function_declaration_t>());
        break;
    }

    default:
        break;
    }

    throw compile_error_t("unknown statement type");
}

jit::value_t compiler_t::build(bool v)
{
    return m_context.new_constant<bool>(v);
}

jit::value_t compiler_t::build(int32_t v)
{
    return m_context.new_constant<int32_t>(v);
}

jit::value_t compiler_t::build(int64_t v)
{
    return m_context.new_constant<int64_t>(v);
}

jit::value_t compiler_t::build(float v)
{
    return m_context.new_constant<float>(v);
}

jit::value_t compiler_t::build(double v){

    return m_context.new_constant<double>(v);
}


jit::value_t compiler_t::build(const ast::expression_t & expression)
{
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



jit::value_t compiler_t::build(const ast::operand_t & operand)
{
    switch (operand.type()) {
    case ast::operand_type_e::i32:
        return build(operand.as<std::int32_t>());
    case ast::operand_type_e::i64:
        return build(operand.as<std::int64_t>());
    case ast::operand_type_e::f32:
        return build(operand.as<float>());
    case ast::operand_type_e::f64:
        return build(operand.as<double>());
    case ast::operand_type_e::variable:{
        jit::value_t var = m_context.main_module().find_variable(
                    operand.as<ast::variable_t>().value
                    );
        if(var)return var;
        throw compile_error_t(
                    "Variable "+operand.as<ast::variable_t>().value+" not found",
                    operand.as<ast::variable_t>()
                    );
    }

    case ast::operand_type_e::unary_expression:
        return build(operand.as<x3::forward_ast<ast::unary_t>>().get());

    case ast::operand_type_e::expression:
        return build(operand.as<x3::forward_ast<ast::expression_t>>().get());

    case ast::operand_type_e::function_call:
        return build(operand.as<x3::forward_ast<ast::function_call_t>>().get());

    default:
        throw compile_error_t("unknown operand");

    }
}



jit::value_t compiler_t::build(const ast::unary_t & expression)
{
    jit::value_t rhs = build(expression.operand);
    switch (expression.op) {
    case ast::operator_type_e::positive:
        return rhs;
    case ast::operator_type_e::negative:
        return -rhs;
    default:
        throw compile_error_t("Unknown unary oprator",expression);
    }
}

jit::value_t compiler_t::build(const ast::function_call_t & func_call)
{
    jit::function_id_t signature;
    signature.return_type = jit::get_basic_type_kind<double>();
    signature.args_type = std::vector<jit::basic_type_kind_e>(
                func_call.arguments.size(),
                jit::get_basic_type_kind<double>()
                );

    signature.name = func_call.name.value;

    jit::function_t called_fun = m_context.main_module().find_function(signature);
    if(!called_fun){
        throw compile_error_t("function "+func_call.name.value+" not found",func_call);
    }

    std::vector<jit::value_data_t> args (func_call.arguments.size());

    size_t i = 0;
    for(const ast::expression_t& expression : func_call.arguments){
        args[i++] = build(expression);
    }

    return m_context.main_module().new_call(called_fun,args);

}


jit::function_t compiler_t::build(const ast::function_declaration_t & function_dec)
{
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

    jit::function_t function = m_context.main_module().new_function(info);

    m_context.main_module().push_scope(function);

    jit::block_t main_block = function.new_block("entry");

    m_context.main_module().push_scope(main_block);

    main_block.set_as_insert_point();
    main_block.set_return(build(function_dec.expression));


    m_context.main_module().pop_scope();
    m_context.main_module().pop_scope();


    auto check =  function.finalize();
    if(!check.first) {
        throw compile_error_t(check.second);
    }

    return function;

}

jit::function_t compiler_t::create_top_level_expression_function(const ast::expression_t & expression)
{
    //TODO
    jit::function_creation_info_t info;
    jit::function_t function = m_context.main_module().new_function(info);
    return function;
}

