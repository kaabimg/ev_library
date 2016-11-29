#include "value.h"
#include "private_data.h"
#include "context.h"

using namespace ev::vm;
using namespace ev::vm::jit;

type_t value_t::type() const
{
    return type_t {d->data->getType()};
}

bool value_t::is_number() const
{
    switch (type().kind()) {
    case type_kind_e::i32:
    case type_kind_e::i64:
    case type_kind_e::r32:
    case type_kind_e::r64:
        return true;
    default: return false;
    }
}

value_data_t value_t::data() const
{
    return d->data;
}

value_t value_t::operator+(const value_t& another)
{        
    std::pair<value_t, value_t> values = cast_types(*this,another);
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateAdd(values.first->data,values.second->data)
                );

}

value_t value_t::operator-(const value_t &another)
{
    std::pair<value_t, value_t> values = cast_types(*this,another);
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateSub(values.first->data,values.second->data)
                );

}

value_t value_t::operator-()
{
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateSub(
                    d->context->interface->new_constant<double>(0).data(),
                    d->data)
                );
}

value_t value_t::operator/(const value_t &another)
{
    std::pair<value_t, value_t> values = cast_types(*this,another);

    if(values.first.type().is_integer()){
        return create_object<value_t>(
                        d->context,
                        d->context->builder.CreateSDiv(
                            values.first->data,
                            values.second->data
                        )
                    );
    }
    else return create_object<value_t>(
                d->context,
                d->context->builder.CreateFDiv(values.first->data,values.second->data)
                );
}

value_t value_t::operator*(const value_t &another)
{
    std::pair<value_t, value_t> values = cast_types(*this,another);

    if(values.first.type().is_integer()) {
        return create_object<value_t>(
                    d->context,
                    d->context->builder.CreateMul(values.first->data,values.second->data)
                    );
    }

    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFMul(values.first->data,values.second->data)
                );
}


using cast_key_t =  std::pair<type_kind_e,type_kind_e>;

namespace std {

  template <>
  struct hash<cast_key_t>
  {
    std::size_t operator()(const cast_key_t& k) const
    {
        return (size_t(k.first) << 32) | size_t(k.second);
    }
  };

}

value_t value_t::cast_to(const type_t & dest_type) const
{

    static const std::unordered_map<cast_key_t,Instruction::CastOps> cast_instructions =
    {
        // int -> int
        {{type_kind_e::i32,type_kind_e::i64},Instruction::SExt},
        {{type_kind_e::i64,type_kind_e::i32},Instruction::Trunc},

        // int -> fp
        {{type_kind_e::i32,type_kind_e::r32},Instruction::SIToFP},
        {{type_kind_e::i64,type_kind_e::r32},Instruction::SIToFP},
        {{type_kind_e::i32,type_kind_e::r64},Instruction::SIToFP},
        {{type_kind_e::i64,type_kind_e::r64},Instruction::SIToFP},

        // fp -> int
        {{type_kind_e::r32,type_kind_e::i32},Instruction::FPToSI},
        {{type_kind_e::r64,type_kind_e::i32},Instruction::FPToSI},
        {{type_kind_e::r32,type_kind_e::i64},Instruction::FPToSI},
        {{type_kind_e::r64,type_kind_e::i64},Instruction::FPToSI},
        // fp -> fp
        {{type_kind_e::r32,type_kind_e::r64},Instruction::FPExt},
        {{type_kind_e::r64,type_kind_e::r32},Instruction::FPTrunc},

    };


    type_t my_type = type();
    if(my_type.kind() == dest_type.kind()) return *this;

    auto iter = cast_instructions.find({my_type.kind(),dest_type.kind()});
    if(iter != cast_instructions.end()) {
        return create_object<value_t>(
                    d->context,
                    d->context->builder.CreateCast(iter->second,d->data,dest_type.data())
                    );
    }
    //TODO error
    return *this;
}

std::pair<value_t, value_t> value_t::cast_types(const value_t &v1, const value_t &v2)
{
    type_t t1 = v1.type(),t2 = v2.type();

    if( t1.kind() == t2.kind()) return {v1,v2};
    if(t1.kind() < t2.kind()) {
       return {v1.cast_to(t2),v2};
    }
    else  {
        return {v1,v2.cast_to(t1)};
    }
}
