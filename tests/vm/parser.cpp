#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ev/vm/parser/parser.hpp>
#include <ev/vm/parser/ast.hpp>

#include <float.h>

using namespace ev::vm;
namespace x3 = boost::spirit::x3;

TEST_CASE("parser::integer")
{
    parser p;

    try {
        int32_t value = 33;
        parser_result ast = p.parse(std::to_string(value));

        ast::statement& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        REQUIRE(number.type() == ast::number_type::i32);

        REQUIRE(number.get<int32_t>() == value);
    }
    catch (const std::exception& e) {
        FAIL(e.what());
    }
}

TEST_CASE("parser::long")
{
    parser p;

    try {
        int64_t value = 21474836470;
        parser_result ast = p.parse(std::to_string(value));

        ast::statement& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        REQUIRE(number.type() == ast::number_type::i64);

        REQUIRE(number.get<int64_t>() == value);
    }
    catch (const std::exception& e) {
        FAIL(e.what());
    }
}

TEST_CASE("parser::float")
{
    parser p;

    try {
        float value = 1.43;
        ;

        parser_result ast = p.parse(std::to_string(value));

        ast::statement& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        REQUIRE(number.type() == ast::number_type::r32);

        REQUIRE(number.get<float>() == value);
    }
    catch (const std::exception& e) {
        FAIL(e.what());
    }
}

TEST_CASE("parser::double")
{
    parser p;

    try {
        parser_result ast = p.parse(std::to_string(DBL_MAX));
        ast::statement& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type::expression);

        ast::expression& expression = statement.as<ast::expression>();
        ast::operand& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::expression);

        expression = operand.as<x3::forward_ast<ast::expression>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type::number);

        ast::number& number = operand.as<ast::number>();
        REQUIRE(number.type() == ast::number_type::r64);

        REQUIRE(number.get<double>() == DBL_MAX);
    }
    catch (const std::exception& e) {
        FAIL(e.what());
    }
}
