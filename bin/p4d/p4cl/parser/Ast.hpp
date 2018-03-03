#pragma once

#include <p4cl/P4CLang.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <vector>

#define P4D_CL_USE_DEFAULT_VAR_CO(Type)     \
    Type() = default;                       \
    Type(const Type&) = default;            \
    Type(Type&&) = default;                 \
    Type& operator=(const Type&) = default; \
    Type& operator=(Type&&) = default;

namespace p4cl {
namespace ast {

namespace x3 = boost::spirit::x3;

template <typename... T>
using variant = x3::variant<T...>;

struct Identifier : p4cl::TaggedPosition {
    Identifier() = default;
    Identifier(const std::string& str) : value(str)
    {
    }
    std::string value;
};

struct String : p4cl::TaggedPosition {
    String() = default;
    String(const std::string& str) : value(str)
    {
    }
    std::string value;
};

struct End : p4cl::TaggedPosition {
    End() = default;
    End(const std::string& str) : value(str)
    {
    }
    std::string value;
};

struct Number : p4cl::TaggedPosition {
    variant<int64_t, double> value;
};

struct ExpressionArray;
struct ExpressionPath;
struct FunctionCall;

struct Expression : public variant<Number,
                                   String,
                                   Identifier,
                                   x3::forward_ast<ExpressionArray>,
                                   x3::forward_ast<ExpressionPath>,
                                   x3::forward_ast<FunctionCall>>,
                    p4cl::TaggedPosition {
    P4D_CL_USE_DEFAULT_VAR_CO(Expression)

    using base_type::base_type;
    using base_type::operator=;
};

struct ExpressionArray : p4cl::TaggedPosition {
    std::vector<Expression> expressions;
};

struct ExpressionPath : p4cl::TaggedPosition {
    std::vector<Identifier> ids;
};

struct FunctionCall : p4cl::TaggedPosition {
    ExpressionPath id;
    std::vector<Expression> args;
};

struct Assignment : p4cl::TaggedPosition {
    Identifier id;
    Expression expression;
};

struct ObjectBegin : p4cl::TaggedPosition {
    Identifier type;
    Identifier id;
};

struct AnonymousObjectBegin : p4cl::TaggedPosition {
    Identifier type;
};

struct ObjectEnd : p4cl::TaggedPosition {
};

struct ProgramStatement : public variant<Assignment, ObjectBegin, AnonymousObjectBegin, ObjectEnd>,
                          p4cl::TaggedPosition {
    P4D_CL_USE_DEFAULT_VAR_CO(ProgramStatement)
    using base_type::base_type;
    using base_type::operator=;

    // TODO use a visitor
    bool isEnd() const
    {
        return get().which() == 3;
    }
};
}
}
