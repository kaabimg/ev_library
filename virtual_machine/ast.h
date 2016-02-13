#ifndef EV_AST_H
#define EV_AST_H


#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>


namespace ev { namespace vm { namespace ast {


namespace x3 = boost::spirit::x3;


struct unary_t;
struct expression_t;
struct identifier_t;
struct function_call_t;
struct array_t;
struct object_t;



struct identifier_t : x3::position_tagged
{
    identifier_t(const std::string & name = "") : value(name) {}
    std::string value;
};

struct variable_t : identifier_t {
    variable_t(const std::string & name = "") : identifier_t(name) {}

};


enum class operand_type_e {
    number,
    variable,
    unary_expression,
    expression,
    function_call,
    array,
    object
};


struct operand_t : x3::variant<
        double ,
        variable_t,
        x3::forward_ast<unary_t>,
        x3::forward_ast<expression_t>,
        x3::forward_ast<function_call_t>,
        x3::forward_ast<array_t>,
        x3::forward_ast<object_t>
        >
{
    using base_type::base_type;
    using base_type::operator=;

    operand_type_e type()const{
        return static_cast<operand_type_e>(get().which());
    }
    template <typename T>
    T & as(){
        return boost::get<T>(get());
    }
    template <typename T>
    const T & as()const{
        return boost::get<T>(get());
    }
};





enum class operator_type_e
{
    plus,
    minus,
    times,
    divide,
    positive,
    negative,
};

struct unary_t  : x3::position_tagged
{
    operator_type_e op;
    operand_t operand;
};



struct operation_t : x3::position_tagged
{
    operator_type_e op;
    operand_t operand;
};



struct expression_t : x3::position_tagged
{
    operand_t first;
    std::vector<operation_t> rest;
};



struct function_call_t  : x3::position_tagged{
    identifier_t name;
    std::vector<expression_t> arguments;
};


struct anonymous_function_declaration_t : x3::position_tagged{
    std::vector<identifier_t> arguments;
    expression_t expression;
};


struct array_t : x3::position_tagged{
    std::vector<expression_t> values;
};


struct field_t : x3::position_tagged {
    identifier_t name;
    expression_t value;
};

struct object_t : x3::position_tagged {
    std::vector<field_t> members;
};

struct function_declaration_t : x3::position_tagged{
    identifier_t name;
    std::vector<identifier_t> arguments;
    expression_t expression;

};

struct assignement_t : x3::position_tagged {
    identifier_t variable_name;
    expression_t expression;

};

struct variable_declaration_t : assignement_t {};




enum class statement_type_e {
    variable_declaration,
    function_declaration,
    anonymous_function_declaration,
    assignement,
    expression
};

struct statement_t : x3::variant <
        variable_declaration_t,
        function_declaration_t,
        anonymous_function_declaration_t,
        assignement_t,
        expression_t
        >
{
    using base_type::base_type;
    using base_type::operator=;


    statement_type_e type()const{
        return static_cast<statement_type_e>(get().which());
    }

    template <typename T>
    T & as(){
        return boost::get<T>(get());
    }
    template <typename T>
    const T & as()const{
        return boost::get<T>(get());
    }
};


}}}



#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::identifier_t,value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::variable_t,value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::unary_t,op,operand)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::operation_t,op,operand)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::expression_t,first,rest)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::function_call_t,name,arguments)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::anonymous_function_declaration_t,arguments,expression)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::function_declaration_t,name,arguments,expression)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::assignement_t,variable_name,expression)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::variable_declaration_t,variable_name,expression)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::array_t,values)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::field_t,name,value)
BOOST_FUSION_ADAPT_STRUCT(ev::vm::ast::object_t,members)


#endif // AST_H

