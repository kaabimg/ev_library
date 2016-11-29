#pragma once


#include <boost/fusion/include/adapt_struct.hpp>

#include "ast.h"

BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::number_t,value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::identifier_t,value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::variable_t,value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::unary_t,op,operand)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::operation_t,op,operand)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::expression_t,first,rest)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::function_call_t,name,arguments)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::anonymous_function_declaration_t,arguments,return_type,expression)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::function_declaration_t,name,arguments,return_type,expression)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::variable_declaration_t,type_name,variable_name)
