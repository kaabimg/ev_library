#pragma once

#include "../vm_types.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

namespace ev
{
namespace vm
{
namespace jit
{
struct function_signature
{
    std::string return_type;
    std::vector<std::string> arg_types;
    bool operator==(const function_signature& another) const;
    bool operator!=(const function_signature& another) const;

    std::string to_string() const;
};

struct function_id : function_signature
{
    std::string name;
    bool operator==(const function_id& another) const;
    bool operator!=(const function_id& another) const;
};

struct function_creation_info : function_id
{
    std::vector<std::string> arg_names;
};

namespace detail
{
inline void build_signature(function_signature&, size_t)
{
}

template <typename H>
inline void build_signature(function_signature& signature, size_t pos)
{
    auto iter = builtin_type_names.find(get_type_kind<H>());
    if (iter != builtin_type_names.end()) {
        signature.arg_types[pos] = iter->second;
    }
    else
    {
        signature.arg_types[pos] = "unknown";
    }
}
template <typename H, typename H2, typename... T>
inline void build_signature(function_signature& signature, size_t pos)
{
    auto iter = builtin_type_names.find(get_type_kind<H>());
    if (iter != builtin_type_names.end()) {
        signature.arg_types[pos] = iter->second;
    }
    else
    {
        signature.arg_types[pos] = "unknown";
    }
    build_signature<H2, T...>(signature, ++pos);
}

template <typename R, typename... Args>
inline function_signature build_signature()
{
    function_signature signature;
    signature.return_type = get_type_kind<R>();
    signature.arg_types.resize(sizeof...(Args));
    build_signature<Args...>(signature, 0);
    return signature;
}

}  // detail

template <typename Sig>
struct signature_builder_t;

template <typename R, typename... Args>
struct signature_builder_t<R(Args...)>
{
    typedef R (*FunctionType)(Args...);
    static const function_signature& value()
    {
        static const function_signature signature =
            detail::build_signature<R, Args...>();
        return signature;
    }
};

template <typename Sig>
inline function_signature create_function_signature()
{
    return signature_builder_t<Sig>::value();
}
}
}
}  // namespace ev::vm::jit
