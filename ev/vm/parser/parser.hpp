#pragma once

#include <ev/core/preprocessor.hpp>
#include <memory>

namespace ev {
namespace vm {
namespace ast {
struct statement;
}

struct parser_result {
    std::shared_ptr<ast::statement> statement;
};

struct parser {
    parser();
    ~parser();
    parser_result parse(const std::string&);

private:
    EV_PRIVATE(parser)
};
}
}
