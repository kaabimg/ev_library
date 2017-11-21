#define BOOST_TEST_MODULE parser
#include <boost/test/included/unit_test.hpp>

#include <ev/vm/parser/parser.hpp>
#include <ev/vm/parser/ast.hpp>

#include <float.h>

using namespace ev::vm;
namespace x3 = boost::spirit::x3;

BOOST_AUTO_TEST_CASE(integer)
{
    parser p;

    try {
        int32_t value = 33;
        parser_result ast = p.parse(std::to_string(value));

        ast::statement& statement = *ast.statement.get();
        BOOST_REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        BOOST_REQUIRE(number.type() == ast::number_type::i32);

        BOOST_REQUIRE(number.get<int32_t>() == value);
    }
    catch (const std::exception& e) {
        BOOST_FAIL(e.what());
    }
}

BOOST_AUTO_TEST_CASE(long_)
{
    parser p;

    try {
        int64_t value = 21474836470;
        parser_result ast = p.parse(std::to_string(value));

        ast::statement& statement = *ast.statement.get();
        BOOST_REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        BOOST_REQUIRE(number.type() == ast::number_type::i64);

        BOOST_REQUIRE(number.get<int64_t>() == value);
    }
    catch (const std::exception& e) {
        BOOST_FAIL(e.what());
    }
}

BOOST_AUTO_TEST_CASE(float_)
{
    parser p;

    try {
        float value = 1.43;
        ;

        parser_result ast = p.parse(std::to_string(value));

        ast::statement& statement = *ast.statement.get();
        BOOST_REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        BOOST_REQUIRE(number.type() == ast::number_type::r32);

        BOOST_REQUIRE(number.get<float>() == value);
    }
    catch (const std::exception& e) {
        BOOST_FAIL(e.what());
    }
}

BOOST_AUTO_TEST_CASE(double_)
{
    parser p;

    try {
        parser_result ast = p.parse(std::to_string(DBL_MAX));
        ast::statement& statement = *ast.statement.get();
        BOOST_REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        BOOST_REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        BOOST_REQUIRE(number.type() == ast::number_type::r64);

        BOOST_REQUIRE(number.get<double>() == DBL_MAX);
    }
    catch (const std::exception& e) {
        BOOST_FAIL(e.what());
    }
}
