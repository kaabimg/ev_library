#ifndef EV_JIT_DEBUG_H
#define EV_JIT_DEBUG_H

#include <string>

namespace ev { namespace vm {
struct function_t;
struct value_t;

struct debug {
    static void dump(const function_t &);
    static void dump(const value_t &, const std::string & name);
};


}

}


#endif // EV_JIT_DEBUG_H
