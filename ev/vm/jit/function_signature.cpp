
#include "function_signature.hpp"

using namespace ev::vm::jit;

bool function_signature::operator==(const function_signature& another) const
{
    if (return_type != another.return_type ||
        arg_types.size() != another.arg_types.size())
    {
        return false;
    }

    for (size_t i = 0; i < arg_types.size(); ++i) {
        if (arg_types[i] != another.arg_types[i]) return false;
    }
    return true;
}

bool function_signature::operator!=(const function_signature& another) const
{
    return !(*this == another);
}

std::string function_signature::to_string() const
{
    std::string str;

    str.append(return_type);
    str.append("(");

    if (arg_types.size()) {
        for (int i = 0; i < arg_types.size() - 1; ++i) {
            str.append(arg_types[i]);
            str.append(",");
        }
        str.append(*arg_types.rbegin());
    }

    str.append(")");

    return str;
}

bool function_id::operator==(const function_id& another) const
{
    return name == another.name && function_signature::operator==(another);
}

bool function_id::operator!=(const function_id& another) const
{
    return !(*this == another);
}
