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

namespace ev
{
namespace vm
{
struct virtual_machine::data
{
    parser par;
    jit::context context;
    compiler_t compiler{context};
};
}
}

virtual_machine::virtual_machine() : d(new virtual_machine::data)
{
}

virtual_machine::~virtual_machine()
{
    delete d;
}

void virtual_machine::eval(const std::string& line)
{
    if (line.size() && *line.begin() == '#') {
        return;
    }
    parser_result result   = d->par.parse(line);
    jit::function function = d->compiler.compile(*result.statement.get());
    d->context.compile();

    if (function &&
        result.statement->type() == ast::statement_type::expression)
    {
        runtime_function<double()> f{function.d.get()};
        std::cout << f() << std::endl;
    }
}

void* virtual_machine::create_function(
    const std::string& str,
    const jit::function_signature& expected_signature)
{
    parser_result result = d->par.parse(str);

    if (result.statement->type() != ast::statement_type::function_declaration)
    {
        throw syntax_error_t("Expected a function definition");
    }

    jit::function function = d->compiler.compile(*result.statement.get());

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
