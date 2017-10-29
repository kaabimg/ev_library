#pragma once

#include <unordered_map>
namespace ev {
namespace vm {
enum class type_kind : uint8_t { unknown = 0, boolean, i32, i64, r32, r64, structure };

static const std::unordered_map<std::string, type_kind> builtin_type_ids = {
    {"bool", type_kind::boolean},
    {"int32", type_kind::i32},
    {"int64", type_kind::i64},
    {"real32", type_kind::r32},
    {"real64", type_kind::r64}};

static const std::unordered_map<type_kind, std::string> builtin_type_names = {
    {type_kind::boolean, "bool"},
    {type_kind::i32, "int32"},
    {type_kind::i64, "int64"},
    {type_kind::r32, "real32"},
    {type_kind::r64, "real64"}};

template <typename T>
constexpr type_kind get_type_kind()
{
    return type_kind::unknown;
}

#define EV_VM_DECLARE_BASIC_TYPE(type, kind)         \
    template <>                                      \
    inline constexpr type_kind get_type_kind<type>() \
    {                                                \
        return type_kind::kind;                      \
    }

EV_VM_DECLARE_BASIC_TYPE(bool, boolean)
EV_VM_DECLARE_BASIC_TYPE(std::int32_t, i32)
EV_VM_DECLARE_BASIC_TYPE(std::int64_t, i64)
EV_VM_DECLARE_BASIC_TYPE(float, r32)
EV_VM_DECLARE_BASIC_TYPE(double, r64)
}
}
