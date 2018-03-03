#include "Parser.hpp"
#include "AstAdapted.hpp"
#include "ProgramTree.hpp"

#include <ev/core/string_utils.hpp>
#include <boost/spirit/home/x3.hpp>

#include <map>
#include <sstream>

namespace x3 = boost::spirit::x3;
namespace ast = p4cl::ast;

namespace {
namespace rules {

struct PositionAnnotator {
    template <typename T, typename Iterator, typename Context>
    inline void on_success(Iterator const& first,
                           Iterator const& last,
                           T& ast,
                           Context const& /*context*/)
    {
        ast.annotate(first, last);
    }
};

// rules
const x3::rule<struct IdType, std::string> idImpl = "id";
const x3::rule<PositionAnnotator, ast::Identifier> identifier = "identifier";
const x3::rule<PositionAnnotator, ast::String> string = "string";
const x3::rule<PositionAnnotator, ast::Number> number = "number";
const x3::rule<PositionAnnotator, ast::Expression> expression = "expression";
const x3::rule<PositionAnnotator, ast::ExpressionArray> expressionArray = "expressionArray";
const x3::rule<PositionAnnotator, ast::ExpressionPath> expressionPath = "expressionPath";
const x3::rule<PositionAnnotator, ast::FunctionCall> functionCall = "functionCall";

const x3::rule<PositionAnnotator, ast::Assignment> assignment = "assignment";

const x3::rule<PositionAnnotator, ast::ObjectBegin> objectBegin = "objectbegin";
const x3::rule<PositionAnnotator, ast::AnonymousObjectBegin> anonymousObjectBegin =
    "anonymousObjectBegin";

const x3::rule<PositionAnnotator, ast::ObjectEnd> objectEnd = "objectEnd";

const x3::rule<PositionAnnotator, ast::ProgramStatement> programStatement = "programStatement";

const struct EndObjectKw : x3::symbols<x3::unused_type> {
    EndObjectKw()
    {
        add("end", x3::unused);
    }
} endObjectKw;

using x3::alnum;
using x3::alpha;
using x3::lexeme;
using x3::lit;
using x3::raw;

const auto& any_char = x3::ascii::char_;

const auto idImpl_def = raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

const auto identifier_def = idImpl;
const auto variable_def = idImpl;

const auto string_def =
    lexeme['"' >> -(+(any_char - '"')) >> '"'] | lexeme['\'' >> -(+(any_char - '\'')) >> '\''];

const auto number_def = x3::real_parser<double, x3::strict_real_policies<double>>{} | x3::int64;

const auto expression_def =
    number | string | functionCall | expressionPath | expressionArray | '(' >> expression >> ')';

const auto expressionArray_def = '[' >> -(expression % ',') >> ']';

const auto expressionPath_def = identifier % '.';

const auto functionCall_def = expressionPath >> '(' >> -(expression % ',') >> ')';

const auto assignment_def = identifier >> '=' >> expression;

const auto objectBegin_def = identifier >> identifier >> ':';
const auto anonymousObjectBegin_def = identifier >> ':';

const auto objectEnd_def = endObjectKw;

auto const skipper = x3::space | x3::lexeme['#' >> *(x3::char_ - x3::eol) >> -x3::eol];

// clang-format off

const auto programStatement_def =
    objectBegin |
    anonymousObjectBegin |
    objectEnd |
    assignment ;

BOOST_SPIRIT_DEFINE(
    idImpl,
    identifier,
    string,
    number,
    expression,
    expressionArray,
    expressionPath,
    functionCall,
    assignment,
    objectBegin,
    anonymousObjectBegin,
    objectEnd, 
    programStatement
)
// clang-format on
}
}

bool p4cl::TextPosition::isValid() const
{
    return begin > -1 && end >= begin;
}

size_t p4cl::TextPosition::size() const
{
    if (!isValid()) return 0;
    return size_t(end) - size_t(begin);
}

std::string_view p4cl::parser::Result::textOf(const p4cl::ast::Node* node) const
{
    return node->position.strView;
}

p4cl::TextPosition p4cl::parser::Result::positionOf(const p4cl::ast::Node* node) const
{
    if (!isValid() || !node->position.isValidPosition()) return {};

    return {std::distance(code->c_str(), node->position.strView.begin()),
            std::distance(code->c_str(), node->position.strView.end())};
}

p4cl::TextPosition p4cl::parser::Result::positionOf(const TaggedPosition& position) const
{
    if (!isValid() || !position.isValidPosition()) return {};

    return {std::distance(code->c_str(), position.strView.begin()),
            std::distance(code->c_str(), position.strView.end())};
}

bool p4cl::parser::Result::isValid() const
{
    return rootNode.get() != nullptr;
}

p4cl::parser::LineParseStatus p4cl::parser::parse(const char* begin,
                                                  const char* end,
                                                  ast::ProgramStatement& statement)
{
    using p4cl::parser::LineParseStatus;

    bool ok = x3::phrase_parse(begin, end, rules::programStatement, rules::skipper, statement);

    if (ok && begin == end)
        return LineParseStatus::Success;
    else if (begin == end)
        return LineParseStatus::EmptyLine;
    else
        return LineParseStatus::Error;
}

p4cl::parser::Result p4cl::parser::parse(std::string program, lang::CompilationMode mode)
{
    p4cl::parser::Result result;
    result.code = std::make_shared<std::string>(std::move(program));
    result.log = std::make_shared<lang::CompilationLog>();
    auto lines = ev::split_str(*result.code, '\n');

    parser::LineParseStatus lps;

    p4cl::ast::ProgramTreeBuilder builder;
    bool ok;
    for (auto line : lines) {
        p4cl::ast::ProgramStatement stmt;

        lps = p4cl::parser::parse(line.begin(), line.end(), stmt);
        if (builder.isInScriptNode()) {
            if (lps == LineParseStatus::Success && stmt.isEnd()) {
                ok = stmt.apply_visitor(builder);
                if (!ok) {
                    std::string_view trimmed = line;
                    trimmed.remove_prefix(std::min(trimmed.find_first_not_of(" "), trimmed.size()));
                    trimmed.remove_prefix(
                        std::min(trimmed.find_first_not_of('\t'), trimmed.size()));

                    result.addParsingError(trimmed);
                    if (mode != lang::CompilationMode::Permissive) {
                        result.rootNode = builder.build();
                        return result;
                    }
                    continue;
                }
            }
            else {
                builder.addScriptLine(line);
            }
        }
        else {
            if (lps == LineParseStatus::EmptyLine) continue;
            if (lps == LineParseStatus::Error) {
                std::string_view trimmed = line;
                trimmed.remove_prefix(std::min(trimmed.find_first_not_of(" "), trimmed.size()));
                trimmed.remove_prefix(std::min(trimmed.find_first_not_of('\t'), trimmed.size()));

                result.addParsingError(trimmed);
                if (mode != lang::CompilationMode::Permissive) {
                    result.rootNode = builder.build();
                    return result;
                }
                continue;
            }

            if (!stmt.apply_visitor(builder)) {
                result.addParsingError(stmt.strView, "Invalid statement");
                if (mode != lang::CompilationMode::Permissive) {
                    result.rootNode = builder.build();
                    return result;
                }
            }
        }
    }
    result.rootNode = builder.build();
    return result;
}
