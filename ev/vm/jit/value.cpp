#include "value.h"
#include "private_data.h"
#include "context.h"

using namespace ev::vm::jit;

value_t value_t::operator+(const value_t& another)
{
    value_t val;
    val.m_data = std::make_shared<value_private_t>(
                m_data->context,
                m_data->context->builder.CreateFAdd(m_data->data,another.m_data->data)
                );

    return create_object<value_t>(
                m_data->context,
                m_data->context->builder.CreateFAdd(m_data->data,another.m_data->data)
                );

}

value_t value_t::operator-(const value_t &another)
{
    return create_object<value_t>(
                m_data->context,
                m_data->context->builder.CreateFSub(m_data->data,another.m_data->data)
                );

}

value_t value_t::operator-()
{
    return create_object<value_t>(
                m_data->context,
                m_data->context->builder.CreateFSub(
                                m_data->context->interface->new_constant<double>(0),
                                m_data->data)
                );
}

value_t value_t::operator/(const value_t &another)
{
    return create_object<value_t>(
                m_data->context,
                m_data->context->builder.CreateFDiv(m_data->data,another.m_data->data)
                );
}

value_t value_t::operator*(const value_t &another)
{
    return create_object<value_t>(
                m_data->context,
                m_data->context->builder.CreateFMul(m_data->data,another.m_data->data)
                );
}

value_t::operator value_data_t() const
{
    return m_data->data;
}
