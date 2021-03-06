
//#define BOOST_SPIRIT_X3_DEBUG

#include "parser.hpp"
#include "ast_adapted.hpp"
#include "../vm_exceptions.hpp"

#include <iostream>

#include <boost/spirit/home/x3.hpp>

using namespace ev::vm;

namespace x3 = boost::spirit::x3;

namespace ev {
namespace vm {
using x3::raw;
using x3::lexeme;
using x3::alpha;
using x3::alnum;

namespace rules {
// operators
x3::symbols<ast::operator_type> add_operator, multiply_operator, unary_operator;

// rules

const x3::rule<struct id_type, std::string> id = "id";
const x3::rule<struct identifier_type, ast::identifier> identifier = "identifier";
const x3::rule<struct variable_type, ast::variable> variable = "variable";
const x3::rule<struct number_type, ast::number> number = "number";

const x3::rule<struct additive_expression_type, ast::expression> additive_expression =
    "additive_expression";

const x3::rule<struct multiplicative_expression_type, ast::expression> multiplicative_expression =
    "multiplicative_expression";

const x3::rule<struct unary_expression_type, ast::operand> unary_expression = "unary_expression";

const x3::rule<struct primary_expression_type, ast::operand> primary_expression =
    "primary_expression";

const x3::rule<struct function_call_type, ast::function_call> function_call = "function_call";

const x3::rule<struct expression_type, ast::expression> expression = "expression";

const x3::rule<struct function_declaration_type, ast::function_declaration> function_declaration =
    "function_declaration";

const x3::rule<struct anonymous_function_declaration_type, ast::anonymous_function_declaration>
    anonymous_function_declaration = "anonymous_function_declaration";

const x3::rule<struct variable_declaration_type, ast::variable_declaration> variable_declaration =
    "variable_declaration";

const x3::rule<struct struct_type, ast::structure> structure = "structure";

// struct statement_type;
const x3::rule<struct error_handler_t, ast::statement> statement = "statement";

const auto id_def = raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

const auto identifier_def = id;
const auto variable_def = id;

const auto number_def = x3::real_parser<float, x3::strict_real_policies<float>>{} |
                        x3::real_parser<double, x3::strict_real_policies<double>>{} | x3::int32 |
                        x3::int64;

auto const additive_expression_def = multiplicative_expression >>
                                     *(add_operator >> multiplicative_expression);

auto const multiplicative_expression_def = unary_expression >>
                                           *(multiply_operator >> unary_expression);

auto const unary_expression_def = primary_expression | (unary_operator >> primary_expression);

auto const function_call_def = identifier >> '(' > (expression % ',') >> ')';

auto const primary_expression_def = number | function_call | variable | ('(' >> expression >> ')');

auto const expression_def = additive_expression;

auto const function_declaration_def = identifier >> '(' >> variable_declaration % ',' >> ')' >>
                                      ':' >> identifier >> '=' >> expression;

auto const anonymous_function_declaration_def =
    '(' >> variable_declaration % ',' >> ')' >> ':' >> identifier >> '=' >> expression;

auto const assignement_def = identifier >> '=' >> expression;

auto const variable_declaration_def = identifier >> identifier;

auto const array_def = "[" >> -(expression % ',') >> "]";

auto const object_def = "{" >> -((identifier >> ":" >> expression) % ',') >> "}";

auto const struct_def = "struct" >> identifier >> "{" >> variable_declaration % ';' >> "}";

auto const statement_def = function_declaration | anonymous_function_declaration | expression;

BOOST_SPIRIT_DEFINE(id,
                    identifier,
                    number,
                    expression,
                    unary_expression,
                    additive_expression,
                    multiplicative_expression,
                    primary_expression,
                    variable,
                    function_call,
                    function_declaration,
                    anonymous_function_declaration,
                    variable_declaration,
                    statement);

#include <sstream>

struct error_handler_t {
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result on_error(Iterator&,
                                      Iterator const& last,
                                      Exception const& x,
                                      Context const& /*context*/)
    {
        std::stringstream ss;
        ss << "Error! Expecting: " << x.which() << " here: \"" << std::string(x.where(), last)
           << "\"";

        throw syntax_error_t(ss.str());

        // return x3::error_handler_result::fail;
    }
};

inline bool init_operators()
{
    add_operator.add("+", ast::operator_type::plus)("-", ast::operator_type::minus);

    multiply_operator.add("*", ast::operator_type::times)("/", ast::operator_type::divide);

    unary_operator.add("+", ast::operator_type::positive)("-", ast::operator_type::negative);

    return true;
}

inline const auto& main_rule()
{
    static bool i = init_operators();
    ev_unused(i);

    return statement;
}
}

struct parser::data {
    const x3::rule<rules::error_handler_t, ast::statement>& main_rule = rules::main_rule();
};
}
}
parser::parser() : d(new data)
{
}

parser::~parser()
{
    delete d;
}

parser_result parser::parse(const std::string& line)
{
    parser_result result;

    std::string::const_iterator begin = line.begin();
    std::string::const_iterator end = line.end();

    boost::spirit::x3::ascii::space_type space;

    ast::statement statement;

    x3::phrase_parse(begin, end, d->main_rule, space, statement);
    if (begin == end) {
        result.statement = std::make_shared<ast::statement>(std::move(statement));
        return result;
    }
    throw syntax_error_t("Syntax error");
}
