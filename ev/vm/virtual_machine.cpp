#include "virtual_machine.hpp"

#include "parser/ast.hpp"
#include "parser/parser.hpp"
#include "compiler/compiler.hpp"
#include "jit/context.hpp"
#include "jit/function.hpp"
#include "jit/private_data.hpp"
#include "vm_exceptions.hpp"

#include <iostream>
#include <memory>
#include <unordered_set>

using namespace ev::vm;

namespace ev {
namespace vm {

struct virtual_machine_t::data_t {
    parser_t parser;
    jit::context_t context;
    compiler_t compiler{context};
};
}
}

virtual_machine_t::virtual_machine_t() : d(new virtual_machine_t::data_t) {}

virtual_machine_t::~virtual_machine_t() {
    delete d;
}

void virtual_machine_t::eval(const std::string& line) {
    if (line.size() && *line.begin() == '#') { return; }
    parser_result_t result = d->parser.parse(line);
    jit::function_t function = d->compiler.compile(*result.statement.get());
    d->context.compile();

    if (function &&
        result.statement->type() == ast::statement_type_e::expression) {
        runtime_function_t<double()> f{function.d.get()};
        std::cout << f() << std::endl;
    }
}

void* virtual_machine_t::create_function(
    const std::string& str,
    const jit::function_signature_t& expected_signature) {
    parser_result_t result = d->parser.parse(str);

    if (result.statement->type() !=
        ast::statement_type_e::function_declaration) {
        throw syntax_error_t("Expected a function definition");
    }

    jit::function_t function = d->compiler.compile(*result.statement.get());

    if (expected_signature == function.creation_info()) {
        d->context.compile();
        return function.d.get();
    }

    std::string error_str =
        "Expexted signature not matching compiled function\n";
    error_str.append("Expected signature : ")
        .append(expected_signature.to_string())
        .append("\n");
    error_str.append("Actual   signature : ")
        .append(function.creation_info().to_string());

    throw compile_error_t(error_str);
}
