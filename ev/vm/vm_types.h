#pragma once


namespace ev { namespace vm {



enum class type_kind_e : uint8_t {
    unknown = 0,
    void_t,
    boolean,
    i32,i64,
    r32,r64,
    structure
};

static const std::unordered_map<std::string,type_kind_e> builtin_type_ids = {
    {"void",   type_kind_e::void_t},
    {"bool",type_kind_e::boolean},
    {"i32",    type_kind_e::i32},
    {"i64",    type_kind_e::i64},
    {"r32",    type_kind_e::r32},
    {"r64",    type_kind_e::r64}
};

static const std::unordered_map<type_kind_e,std::string> builtin_type_names = {
    {type_kind_e::void_t  ,"void"},
    {type_kind_e::boolean ,"bool"},
    {type_kind_e::i32     ,"i32"},
    {type_kind_e::i64     ,"i64"},
    {type_kind_e::r32     ,"r32"},
    {type_kind_e::r64     ,"r64"}
};


template <typename T>
constexpr type_kind_e get_type_kind(){
    return type_kind_e::unknown;
}

#define EV_VM_DECLARE_BASIC_TYPE(type,kind) \
template <> \
inline constexpr type_kind_e get_type_kind<type>(){ \
    return type_kind_e::kind; \
}

EV_VM_DECLARE_BASIC_TYPE(void         ,void_t)
EV_VM_DECLARE_BASIC_TYPE(bool         ,boolean)
EV_VM_DECLARE_BASIC_TYPE(std::int32_t ,i32)
EV_VM_DECLARE_BASIC_TYPE(std::int64_t ,i64)
EV_VM_DECLARE_BASIC_TYPE(float        ,r32)
EV_VM_DECLARE_BASIC_TYPE(double       ,r64)


}}
