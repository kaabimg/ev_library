#ifndef EV_VM_JIT_UTILS_H
#define EV_VM_JIT_UTILS_H

#include <string>
#include <vector>
#include <assert.h>


namespace ev { namespace vm { namespace jit {


enum class basic_type_kind_e {
    none,
    boolean,
    i32,i64,
    f32,f64
};


template <typename T>
constexpr basic_type_kind_e get_basic_type_kind(){
    assert(false && "Unhandled type");
}

#define EV_DECLARE_BASIC_TYPE(type,kind) \
template <> \
inline constexpr basic_type_kind_e get_basic_type_kind<type>(){ \
    return basic_type_kind_e::kind; \
}

EV_DECLARE_BASIC_TYPE(bool         ,boolean)
EV_DECLARE_BASIC_TYPE(std::int32_t ,i32)
EV_DECLARE_BASIC_TYPE(std::int64_t ,i64)
EV_DECLARE_BASIC_TYPE(float        ,f32)
EV_DECLARE_BASIC_TYPE(double       ,f64)


struct function_signature_t {
    basic_type_kind_e return_type = basic_type_kind_e::none;
    std::vector<basic_type_kind_e> args_type;
    bool operator==(const function_signature_t & another)const;
    bool operator!=(const function_signature_t & another)const;
};

struct function_id_t : function_signature_t {
    std::string name;
    bool operator==(const function_id_t & another)const;
    bool operator!=(const function_id_t & another)const;
};

struct function_creation_info_t : function_id_t {
    std::vector<std::string> args_names;
};

namespace detail {
inline void build_signature(function_signature_t &,size_t){}

template <typename H>
inline void build_signature(function_signature_t & signature,size_t pos){
    signature.args_type[pos] = get_basic_type_kind<H>();
}
template <typename H,typename H2,typename ...T>
inline void build_signature(function_signature_t & signature,size_t pos){
    signature.args_type[pos] = get_basic_type_kind<H>();
    build_signature<H2,T...>(signature,++pos);
}


template <typename R,typename ...Args>
inline function_signature_t build_signature(){
    function_signature_t signature;
    signature.return_type = get_basic_type_kind<R>();
    signature.args_type.resize(sizeof...(Args));
    build_signature<Args...>(signature,0);
    return signature;
}



} // detail


template <typename Sig>
struct signature_builder_t;

template <typename R,typename ... Args>
struct signature_builder_t<R(Args...)>
{
    typedef R(*FunctionType)(Args...);
    static const function_signature_t & value(){
        static const function_signature_t signature = detail::build_signature<R,Args...>();
        return signature;
    }
};

template <typename Sig>
inline function_signature_t create_function_signature(){
    return signature_builder_t<Sig>::value();
}


}}} // namespace ev::vm::jit

#endif//EV_VM_JIT_UTILS_H
