#pragma once

#include <boost/fusion/include/adapt_struct.hpp>

#include "ast.hpp"

BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::number, value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::identifier, value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::variable, value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::unary, op_type, op)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::operation, op_type, op)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::expression, first, rest)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::function_call, name, arguments)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::anonymous_function_declaration,
                          arguments,
                          return_type,
                          expr)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::function_declaration,
                          name,
                          arguments,
                          return_type,
                          expr)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::variable_declaration, type_name, variable_name)
