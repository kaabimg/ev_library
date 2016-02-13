#include "parser.h"
#include "ast.h"

//#define BOOST_SPIRIT_X3_DEBUG


#include <iostream>

#include <boost/spirit/home/x3.hpp>


using namespace ev::vm;


namespace x3 = boost::spirit::x3;




namespace ev { namespace vm {

using x3::raw;
using x3::lexeme;
using x3::alpha;
using x3::alnum;


namespace rules {




// operators
x3::symbols<ast::operator_type_e> add_operator,multiply_operator,unary_operator;


// rules


const x3::rule <struct id_type , std::string>  id = "id";
const x3::rule <struct identifier_type , ast::identifier_t>  identifier = "identifier";
const x3::rule <struct variable_type, ast::variable_t >  variable = "variable";
const x3::rule <struct number_type ,double>  number = "number";



const x3::rule <struct additive_expression_type, ast::expression_t >
        additive_expression = "additive_expression";

const x3::rule <struct multiplicative_expression_type, ast::expression_t >
        multiplicative_expression = "multiplicative_expression";

const x3::rule <struct unary_expression_type, ast::operand_t>
        unary_expression = "unary_expression";

const x3::rule <struct primary_expression_type,ast::operand_t >
        primary_expression = "primary_expression";

const x3::rule <struct function_call_type ,ast::function_call_t>
        function_call = "function_call";


const x3::rule <struct expression_type, ast::expression_t>
        expression = "expression";

const x3::rule <struct function_declaration_type ,ast::function_declaration_t>
        function_declaration = "function_declaration";

const x3::rule <struct anonymous_function_declaration_type ,ast::anonymous_function_declaration_t>
        anonymous_function_declaration = "anonymous_function_declaration";


const x3::rule <struct variable_declaration_type ,ast::variable_declaration_t>
        variable_declaration = "variable_declaration";

const x3::rule <struct assignement_type ,ast::assignement_t>
        assignement = "assignement";

const x3::rule <struct array_type ,ast::array_t>
        array = "array";

const x3::rule <struct object_type ,ast::object_t>
        object = "object";


//struct statement_type;
const x3::rule <struct statement_type,ast::statement_t >  statement = "statement";

const auto id_def =
        raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

const auto identifier_def = id;
const auto variable_def = id;
const auto number_def = x3::double_;


auto const additive_expression_def =
        multiplicative_expression
        >> *(add_operator >> multiplicative_expression)
           ;


auto const multiplicative_expression_def =
        unary_expression
        >> *(multiply_operator >> unary_expression)
           ;


auto const unary_expression_def =
        primary_expression
        |   (unary_operator >> primary_expression)
        ;



auto const function_call_def =
        identifier >> '(' > (expression % ',') >> ')'
        ;

auto const primary_expression_def =
        number
        | function_call
        | variable
        | ('(' >> expression >> ')')
        | array
        | object



        ;

auto const expression_def = additive_expression;


auto const function_declaration_def =
           identifier
        >> '(' >> identifier % ',' >> ')'
        >> '=' >> expression
        ;

auto const anonymous_function_declaration_def =
          '(' >> identifier % ',' >> ')'
        >> '=' >> expression
        ;

auto const assignement_def =
           identifier  >> '=' >> expression  ;

auto const variable_declaration_def =
        "var" >>  identifier  >> '=' >> expression ;

auto const array_def =
        "[" >>  - (expression % ',') >> "]";


auto const object_def =
        "{" >>  - ( (identifier >> ":" >> expression) % ',') >> "}";




auto const statement_def =
          variable_declaration
        | function_declaration
        | anonymous_function_declaration
        | assignement
        | expression;



BOOST_SPIRIT_DEFINE(
        id,
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
        assignement,
        statement,
        array,
        object
        );

struct statement_type
{
    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result
    on_error(Iterator&, Iterator const& last, Exception const& x, Context const& /*context*/)
    {
        std::cout
                << "Error! Expecting: "
                << x.which()
                << " here: \""
                << std::string(x.where(), last)
                << "\""
                << std::endl
                   ;
        return x3::error_handler_result::fail;
    }
};




inline bool init_operators(){

    add_operator.add
            ("+", ast::operator_type_e::plus)
            ("-", ast::operator_type_e::minus)
            ;

    multiply_operator.add
            ("*", ast::operator_type_e::times)
            ("/", ast::operator_type_e::divide)
            ;


    unary_operator.add
            ("+", ast::operator_type_e::positive)
            ("-", ast::operator_type_e::negative)
            ;


    return true;

}

inline const auto & main_rule (){
    static bool i = init_operators();
    ev_unused(i);

    return statement;
}

}


struct parser_t::data_t {

    const x3::rule <rules::statement_type,ast::statement_t> & main_rule = rules::main_rule();
};


}}
parser_t::parser_t():d(new data_t)
{

}

parser_t::~parser_t()
{
    delete d;
}

parser_result_t parser_t::parse(const std::string& line)
{
    parser_result_t result;


    std::string::const_iterator begin = line.begin();
    std::string::const_iterator end = line.end();

    boost::spirit::x3::ascii::space_type space;



    ast::statement_t statement;

    try {
        x3::phrase_parse(begin,end,d->main_rule,space,statement);
        if( begin == end){
            result.success = true;
            result.statement = std::make_shared<ast::statement_t>(std::move(statement));
        }

    }
    catch(...){
        result.success = false;

    }

    return result;
}
