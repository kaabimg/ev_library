#pragma once

#include <type_traits>

namespace ev {
namespace atk {
namespace detail {
template <typename T>
struct connection_object_trait {
    using type = typename std::remove_cv<typename std::remove_pointer<T>::type>::type;
};
}
}
}

#define ATK_CONNECTION_METHOD(obj, fn) \
    obj, &::ev::atk::detail::connection_object_trait<decltype(obj)>::type::fn


#define ATK_DECLARE_ENUM_QHASH(enum_type) \
    inline uint qHash(enum_type key, uint seed) \
    { \
        return ::qHash(static_cast<uint>(key), seed); \
    }

#define ATK_SIGNAL(obj, fn) ATK_CONNECTION_METHOD(obj, fn)
#define ATK_SLOT(obj, fn) ATK_CONNECTION_METHOD(obj, fn)



#define ATK_ALIAS_QTYPE(qtype,type) \
class qtype; \
namespace ev { namespace atk { using type = qtype; }  }
