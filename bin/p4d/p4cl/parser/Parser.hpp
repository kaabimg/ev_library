#pragma once

#include <p4cl/parser/ProgramTree.hpp>

#include <string>

namespace p4cl {

struct TextPosition {
    long int begin = -1;
    long int end = -1;

public:
    bool isValid() const;
    size_t size() const;
    bool contains(long int pos) const
    {
        return pos >= begin && pos < end;
    }
};

namespace parser {

enum class LineParseStatus { Success, EmptyLine, Error };

struct Result {
    std::shared_ptr<const std::string> code;
    std::shared_ptr<const ast::Node> rootNode;
    std::shared_ptr<lang::CompilationLog> log;

public:
    bool isValid() const;
    std::string_view textOf(const ast::Node*) const;
    TextPosition positionOf(const ast::Node*) const;
    TextPosition positionOf(const TaggedPosition&) const;

public:
    void addParsingError(std::string_view sv)
    {
        if (log) {
            log->push_back({lang::CompilationMessage::ParsingError, TaggedPosition::fromStrView(sv),
                            "Failed to parse line"});
        }
    }

    void addParsingError(std::string_view sv, const std::string& msg)
    {
        if (log) {
            log->push_back(
                {lang::CompilationMessage::ParsingError, TaggedPosition::fromStrView(sv), msg});
        }
    }
};

LineParseStatus parse(const char* begin, const char* end, ast::ProgramStatement& statement);

Result parse(std::string, lang::CompilationMode mode);
}

inline const char* toString(parser::LineParseStatus r)
{
    switch (r) {
        case parser::LineParseStatus::Success: return "Success";
        case parser::LineParseStatus::EmptyLine: return "EmptyLine";
        case parser::LineParseStatus::Error: return "Error";
        default: break;
    }
}
}
