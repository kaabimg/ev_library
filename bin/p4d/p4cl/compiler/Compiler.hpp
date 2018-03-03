#pragma once
#include <p4cl/parser/Parser.hpp>
#include <p4cl/compiler/PipelineGraph.hpp>

namespace p4cl {
namespace compiler {

struct Result {
    parser::Result parserResult;
    std::shared_ptr<const compiler::ProgramNode> program;
    std::shared_ptr<PipelineGraph> pipelineGraph;
    std::shared_ptr<lang::CompilationLog> log;
};

Result compile(parser::Result pr, lang::CompilationMode mode);
Result compile(std::string, lang::CompilationMode mode);
}
}
