#pragma once


#include <ev/core/preprocessor.hpp>
#include <memory>

namespace ev { namespace vm {


namespace ast {
struct statement_t;

}

struct parser_result_t {
    std::shared_ptr<ast::statement_t> statement;
};

struct parser_t {

    parser_t();
    ~parser_t();
    parser_result_t parse(const std::string & );


private:
    EV_PRIVATE(parser_t)
};

}}
