#pragma once
#include <boost/any.hpp>

namespace ev {
using any_t = boost::any;

class property_value_t {
public:
    property_value_t()                        = default;
    property_value_t(const property_value_t&) = default;
    property_value_t(property_value_t&&)      = default;
    property_value_t& operator=(const property_value_t&) = default;
    property_value_t& operator=(property_value_t&&) = default;

    property_value_t(const any_t& any)
    {
        m_value = any;
    }

    operator bool() const
    {
        return !m_value.empty();
    }

    template <typename T>
    property_value_t& operator=(T&& val)
    {
        m_value = std::forward<T>(val);
        return *this;
    }

    template <typename T>
    const T as() const
    {
        return boost::any_cast<const T>(m_value);
    }

    template <typename T>
    T as()
    {
        return boost::any_cast<T>(m_value);
    }

    template <typename T>
    bool is() const
    {
        return boost::any_cast<T>(&m_value);
    }

    any_t value() const
    {
        return m_value;
    }

private:
    any_t m_value;
};

template <typename T>
struct is_trivial_property {
    static constexpr bool value =
        std::is_trivial<T>::value && sizeof(T) <= sizeof(double) * 2;
};

template <typename T>
struct property_trait {
    using type = std::conditional<is_trivial_property<T>::value, T, const T&>;

    template <typename C>
    struct getter {
        typedef property_trait::type (C::*type)() const;
    };

    template <typename C>
    struct setter {
        typedef void (C::*type)(property_trait::type);
    };
};

class object_t;

struct meta_property_t {
    virtual void write(object_t*, const any_t& val) const = 0;
    virtual any_t read(const object_t*) const = 0;
};

template <typename Class,
          typename Property,
          Property (Class::*getter)() const,
          void (Class::*setter)(const Property&)>
struct member_property_t  {
    void write(object_t* obj, const any_t& val)const
    {
        return (static_cast<Class*>(obj)->*setter)(
            boost::any_cast<Property>(val));
    }
    any_t read(const object_t* obj) const
    {
        return (static_cast<const Class*>(obj)->*getter)();
    }
};

}
