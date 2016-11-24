#include "utils.h"


using namespace ev::vm::jit;


bool function_signature_t::operator ==(const function_signature_t &another)const
{
    if( return_type != another.return_type ||
            args_type.size() != another.args_type.size()  )
    {
        return false;
    }

    for (size_t i = 0; i < args_type.size(); ++i) {
        if(args_type[i] != another.args_type[i]) return false;
    }
    return true;

}

bool function_signature_t::operator!=(const function_signature_t &another) const
{
    return !(*this == another);
}

std::string function_signature_t::to_string() const
{
    std::string str;

    str.append(to_string(return_type));
    str.append("(");

    if(args_type.size()){
        for (int i = 0; i < args_type.size()-1; ++i) {
            str.append(to_string(args_type[i]));
            str.append(",");
        }
        str.append(to_string(*args_type.rbegin()));
    }

    str.append(")");


    return str;
}

const char* function_signature_t::to_string(basic_type_kind_e k)
{
    switch (k) {
    default:                         return "unknown";
    case basic_type_kind_e::void_t:  return "void";
    case basic_type_kind_e::boolean: return "boolean";
    case basic_type_kind_e::i32:     return "i32";
    case basic_type_kind_e::i64:     return "i64";
    case basic_type_kind_e::f32:     return "f32";
    case basic_type_kind_e::f64:     return "f64";
    }
}

bool function_id_t::operator==(const function_id_t& another) const
{
    return  name == another.name && function_signature_t::operator ==(another);
}

bool function_id_t::operator!=(const function_id_t& another) const
{
    return !(*this == another);
}
