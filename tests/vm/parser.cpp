#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ev/vm/parser/parser.hpp>
#include <ev/vm/parser/ast.hpp>

#include <float.h>

using namespace ev::vm;
namespace x3 = boost::spirit::x3;

TEST_CASE("parser::integer") {
    parser_t parser;

    try {
        int32_t value = 33;
        parser_result_t ast = parser.parse(std::to_string(value));

        ast::statement_t& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type_e::expression);

        ast::expression_t& expression = statement.as<ast::expression_t>();
        ast::operand_t& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::expression);

        expression = operand.as<x3::forward_ast<ast::expression_t>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::number);

        ast::number_t& number = operand.as<ast::number_t>();
        REQUIRE(number.type() == ast::number_type_e::i32);

        REQUIRE(number.get<int32_t>() == value);

    } catch (const std::exception& e) { FAIL(e.what()); }
}

TEST_CASE("parser::long") {
    parser_t parser;

    try {
        int64_t value = 21474836470;
        parser_result_t ast = parser.parse(std::to_string(value));

        ast::statement_t& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type_e::expression);

        ast::expression_t& expression = statement.as<ast::expression_t>();
        ast::operand_t& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::expression);

        expression = operand.as<x3::forward_ast<ast::expression_t>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::number);

        ast::number_t& number = operand.as<ast::number_t>();
        REQUIRE(number.type() == ast::number_type_e::i64);

        REQUIRE(number.get<int64_t>() == value);

    } catch (const std::exception& e) { FAIL(e.what()); }
}

TEST_CASE("parser::float") {
    parser_t parser;

    try {
        float value = 1.43;
        ;

        parser_result_t ast = parser.parse(std::to_string(value));

        ast::statement_t& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type_e::expression);

        ast::expression_t& expression = statement.as<ast::expression_t>();
        ast::operand_t& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::expression);

        expression = operand.as<x3::forward_ast<ast::expression_t>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::number);

        ast::number_t& number = operand.as<ast::number_t>();
        REQUIRE(number.type() == ast::number_type_e::r32);

        REQUIRE(number.get<float>() == value);

    } catch (const std::exception& e) { FAIL(e.what()); }
}

TEST_CASE("parser::double") {
    parser_t parser;

    try {
        parser_result_t ast = parser.parse(std::to_string(DBL_MAX));
        ast::statement_t& statement = *ast.statement.get();
        REQUIRE(statement.type() == ast::statement_type_e::expression);

        ast::expression_t& expression = statement.as<ast::expression_t>();
        ast::operand_t& operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::expression);

        expression = operand.as<x3::forward_ast<ast::expression_t>>();
        operand = expression.first;
        REQUIRE(operand.type() == ast::operand_type_e::number);

        ast::number_t& number = operand.as<ast::number_t>();
        REQUIRE(number.type() == ast::number_type_e::r64);

        REQUIRE(number.get<double>() == DBL_MAX);

    } catch (const std::exception& e) { FAIL(e.what()); }
}
