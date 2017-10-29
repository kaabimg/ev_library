#pragma once

#include "data_fwd_declare.hpp"
#include "object.hpp"

namespace ev
{
namespace vm
{
namespace jit
{
struct value_private;
struct type;

struct value : object<value_private>
{
    type get_type() const;
    bool is_number() const;

    value_data data() const;

    value operator+(const value& another);
    value operator-(const value& another);
    value operator-();
    value operator/(const value& another);
    value operator*(const value& another);

    value cast_to(const type&) const;

protected:
    std::pair<value, value> cast_types(const value& v1,
                                           const value& v2);

    friend class context;
    friend class function;
    friend class block;
    friend class module;
};

struct named_value
{
    std::string name;
    value val;
};
}
}
}
