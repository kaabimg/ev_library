#pragma once

#include <any>

namespace ev {

class dynamic_property {
public:
    dynamic_property() = default;
    dynamic_property(const dynamic_property&) = default;
    dynamic_property(dynamic_property&&) = default;
    dynamic_property& operator=(const dynamic_property&) = default;
    dynamic_property& operator=(dynamic_property&&) = default;

    dynamic_property(const std::any& any)
    {
        m_value = any;
    }

    operator bool() const
    {
        return m_value.has_value();
    }

    template <typename T>
    dynamic_property& operator=(T&& val)
    {
        m_value = std::forward<T>(val);
        return *this;
    }

    template <typename T>
    const T as() const
    {
        return std::any_cast<const T>(m_value);
    }

    template <typename T>
    T as()
    {
        return std::any_cast<T>(m_value);
    }

    template <typename T>
    bool is() const
    {
        return std::any_cast<T>(&m_value);
    }

    std::any value() const
    {
        return m_value;
    }

private:
    std::any m_value;
};

template <typename T, std::size_t max_size = sizeof(double) * 2>
struct property_trait {
    static constexpr bool is_trivial = std::is_trivial<T>::value && sizeof(T) <= max_size;

    using access_type = typename std::conditional<is_trivial, T, const T&>::type;
};

template <typename T, typename AccessType = typename property_trait<T>::access_type>
class property {
public:
    using property_type = T;
    using access_type = AccessType;

    property()
    {
    }
    property(AccessType v) : m_value(v)
    {
    }

    inline access_type read() const
    {
        return m_value;
    }

    inline void write(access_type v)
    {
        m_value = v;
        notify_changed();
    }

    template <typename V>
    property& operator=(V&& v)
    {
        m_value = std::forward<V>(v);
        notify_changed();
        return *this;
    }

    operator access_type() const
    {
        return m_value;
    }

    template <typename F>
    void on_modified(F&& f)
    {
        m_on_modified = std::forward<F>(f);
    }

protected:
    void notify_changed()
    {
        if (m_on_modified) m_on_modified();
    }

private:
    std::function<void()> m_on_modified = nullptr;
    property_type m_value;
};
}
