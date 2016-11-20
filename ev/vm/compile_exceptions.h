#pragma once


#include <exception>

namespace ev { namespace vm {

struct text_position_t {
    int first = -1,last = -1;
};


struct syntax_error_t {

};

struct compile_error_t  {

};


}}
