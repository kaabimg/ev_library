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

bool function_id_t::operator==(const function_id_t& another) const
{
    return  name == another.name && function_signature_t::operator ==(another);
}

bool function_id_t::operator!=(const function_id_t& another) const
{
    return !(*this == another);
}
