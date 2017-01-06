#pragma once

#include <memory>

namespace ev {
namespace vm {
namespace jit {

template <typename PrivateData>
struct object_t {
    using data_type = PrivateData;

    inline bool is_valid() const { return d.get() != nullptr; }
    inline operator bool() const { return is_valid(); }

    operator PrivateData() const { return d.get(); }
    PrivateData data() const { return d.get(); }

protected:
    object_t() {}
    std::shared_ptr<PrivateData> d{nullptr};

    PrivateData* operator->() { return d.get(); }

    const PrivateData* operator->() const { return d.get(); }

    template <typename T, typename... Arg>
    friend T create_object(Arg&&... args);
};

template <typename T, typename... Arg>
inline T create_object(Arg&&... args) {
    T instance;
    instance.d =
        std::make_shared<typename T::data_type>(std::forward<Arg>(args)...);
    return instance;
}

struct value_t;
struct compilation_scope_t {
    virtual value_t find_variable(const std::string& name) const = 0;
};
}
}
}
