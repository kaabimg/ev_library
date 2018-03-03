#pragma once
#include <p4cl/parser/ProgramTree.hpp>

namespace p4cl {
namespace analysis {

std::vector<p4cl::lang::CompilationMessage> staticAnalyse(const p4cl::ast::Node*);

}
}
