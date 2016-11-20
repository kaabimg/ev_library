#include "value.h"
#include "private_data.h"
#include "context.h"

using namespace ev::vm::jit;

value_t value_t::operator+(const value_t& another)
{
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFAdd(d->data,another.d->data)
                );

}

value_t value_t::operator-(const value_t &another)
{
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFSub(d->data,another.d->data)
                );

}

value_t value_t::operator-()
{
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFSub(
                                d->context->interface->new_constant<double>(0),
                                d->data)
                );
}

value_t value_t::operator/(const value_t &another)
{
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFDiv(d->data,another.d->data)
                );
}

value_t value_t::operator*(const value_t &another)
{
    return create_object<value_t>(
                d->context,
                d->context->builder.CreateFMul(d->data,another.d->data)
                );
}

value_t::operator value_data_t() const
{
    return d->data;
}
