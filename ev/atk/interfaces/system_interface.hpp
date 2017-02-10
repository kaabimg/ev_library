#pragma once

#include "../qtypes_fwd.hpp"

namespace ev {
namespace atk {

struct system_interface_t {
    virtual void info(const qstring&)    = 0;
    virtual void warning(const qstring&) = 0;
    virtual void error(const qstring&)   = 0;
};
}
}
