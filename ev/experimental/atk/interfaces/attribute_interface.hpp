#pragma once

#include "../qtypes_fwd.hpp"
namespace ev {
namespace atk {

template <typename Enum>
struct attribute_interface {
    virtual void set_attribute(Enum att, const qvariant& val) = 0;
    virtual qvariant attribute(Enum) const = 0;
};
}
}
