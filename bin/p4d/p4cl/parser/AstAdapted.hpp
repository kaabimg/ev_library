#pragma once

#include "Ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

// clang-format off
BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::Identifier,
        (std::string ,value)
    )

BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::String,
        (std::string ,value)
    )

BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::Number,
        value
    )
BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::ExpressionArray,
        (std::vector<p4cl::ast::Expression>, expressions)
    )

BOOST_FUSION_ADAPT_STRUCT(p4cl::ast::ExpressionPath, ids)
BOOST_FUSION_ADAPT_STRUCT(p4cl::ast::FunctionCall, id, args)

BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::Assignment,
        (p4cl::ast::Identifier, id)
        (p4cl::ast::Expression, expression)
    )

BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::ObjectBegin,
        (p4cl::ast::Identifier, type)
        (p4cl::ast::Identifier, id)
    )

BOOST_FUSION_ADAPT_STRUCT(
        p4cl::ast::AnonymousObjectBegin,
        (p4cl::ast::Identifier, type)
    )

// clang-format on
