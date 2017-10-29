#include "value.hpp"
#include "private_data.hpp"
#include "context.hpp"

using namespace ev::vm;
using namespace ev::vm::jit;

type value::get_type() const
{
    return type{d->data->getType()};
}

bool value::is_number() const
{
    switch (get_type().kind()) {
        case type_kind::i32:
        case type_kind::i64:
        case type_kind::r32:
        case type_kind::r64: return true;
        default: return false;
    }
}

value_data value::data() const
{
    return d->data;
}

value value::operator+(const value& another)
{
    std::pair<value, value> values = cast_types(*this, another);
    return create_object<value>(
        d->context, d->context->builder.CreateAdd(values.first->data, values.second->data));
}

value value::operator-(const value& another)
{
    std::pair<value, value> values = cast_types(*this, another);
    return create_object<value>(
        d->context, d->context->builder.CreateSub(values.first->data, values.second->data));
}

value value::operator-()
{
    return create_object<value>(
        d->context, d->context->builder.CreateSub(
                        d->context->interface->new_constant<double>(0).data(), d->data));
}

value value::operator/(const value& another)
{
    std::pair<value, value> values = cast_types(*this, another);

    if (values.first.get_type().is_integer()) {
        return create_object<value>(
            d->context, d->context->builder.CreateSDiv(values.first->data, values.second->data));
    }
    else
        return create_object<value>(
            d->context, d->context->builder.CreateFDiv(values.first->data, values.second->data));
}

value value::operator*(const value& another)
{
    std::pair<value, value> values = cast_types(*this, another);

    if (values.first.get_type().is_integer()) {
        return create_object<value>(
            d->context, d->context->builder.CreateMul(values.first->data, values.second->data));
    }

    return create_object<value>(
        d->context, d->context->builder.CreateFMul(values.first->data, values.second->data));
}

using cast_key_t = std::pair<type_kind, type_kind>;

namespace std {
template <>
struct hash<cast_key_t> {
    std::size_t operator()(const cast_key_t& k) const
    {
        return (size_t(k.first) << 32) | size_t(k.second);
    }
};
}

value value::cast_to(const type& dest_type) const
{
    static const std::unordered_map<cast_key_t, Instruction::CastOps> cast_instructions = {
        // int -> int
        {{type_kind::i32, type_kind::i64}, Instruction::SExt},
        {{type_kind::i64, type_kind::i32}, Instruction::Trunc},

        // int -> fp
        {{type_kind::i32, type_kind::r32}, Instruction::SIToFP},
        {{type_kind::i64, type_kind::r32}, Instruction::SIToFP},
        {{type_kind::i32, type_kind::r64}, Instruction::SIToFP},
        {{type_kind::i64, type_kind::r64}, Instruction::SIToFP},

        // fp -> int
        {{type_kind::r32, type_kind::i32}, Instruction::FPToSI},
        {{type_kind::r64, type_kind::i32}, Instruction::FPToSI},
        {{type_kind::r32, type_kind::i64}, Instruction::FPToSI},
        {{type_kind::r64, type_kind::i64}, Instruction::FPToSI},
        // fp -> fp
        {{type_kind::r32, type_kind::r64}, Instruction::FPExt},
        {{type_kind::r64, type_kind::r32}, Instruction::FPTrunc},

    };

    type my_type = get_type();
    if (my_type.kind() == dest_type.kind()) return *this;

    auto iter = cast_instructions.find({my_type.kind(), dest_type.kind()});
    if (iter != cast_instructions.end()) {
        return create_object<value>(
            d->context, d->context->builder.CreateCast(iter->second, d->data, dest_type.data()));
    }
    // TODO error
    return *this;
}

std::pair<value, value> value::cast_types(const value& v1, const value& v2)
{
    type t1 = v1.get_type(), t2 = v2.get_type();

    if (t1.kind() == t2.kind()) return {v1, v2};
    if (t1.kind() < t2.kind()) {
        return {v1.cast_to(t2), v2};
    }
    else {
        return {v1, v2.cast_to(t1)};
    }
}
