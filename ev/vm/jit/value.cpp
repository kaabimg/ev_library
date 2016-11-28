#include "value.h"
#include "private_data.h"
#include "context.h"

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
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFDiv(values.first->data,values.second->data)
                );
}

value_t value_t::operator*(const value_t &another)
{
    std::pair<value_t, value_t> values = cast_types(*this,another);
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateMul(values.first->data,values.second->data)
                );
}

value_t value_t::cast_to(const type_t & dest_type) const
{
    type_t my_type = type();
    if(my_type.kind() == dest_type.kind()) return *this;

    if(my_type.kind() < dest_type.kind()){ //
        if( my_type.is_integer() && dest_type.is_integer()) { // int -> int

        }
        else if(my_type.is_integer() && dest_type.is_floating_point()){ // int -> float

        }
    }
    else {
        if(){ // int -> int

        }
        else if() {// float -> int
        }
    }
}

std::pair<value_t, value_t> value_t::cast_types(const value_t &v1, const value_t &v2)
{
    type_t t1 = v1.type(),t2 = v2.type();

    if( t1.kind() == t2.kind()) return {v1,v2};
    if(t1.kind() < t2.kind()) {
        if(t2.is_floating_point()) {
            return {
                create_object<value_t>(
                            d->context,
                            d->context->builder.CreateCast(Instruction::SIToFP,v1.data(),t2.data())
                            ),
                v2
            };
        }
        else {
            return {
                create_object<value_t>(
                            d->context,
                            d->context->builder.CreateCast(Instruction::SExt,v1.data(),t2.data())
                            ),
                v2
            };
        }
    }
    else  {
        if(t1.is_floating_point()) {
            return {
                v1,
                create_object<value_t>(
                            d->context,
                            d->context->builder.CreateCast(Instruction::SIToFP,v2.data(),t1.data())
                            )
            };
        }
        else {
            return {
                v1,
                create_object<value_t>(
                            d->context,
                            d->context->builder.CreateCast(Instruction::SExt,v2.data(),t1.data())
                            )
            };
        }
    }
}
