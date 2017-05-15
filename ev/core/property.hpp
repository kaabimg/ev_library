#pragma once

#include <experimental/any>

namespace ev {

using any_t = std::experimental::any;

class dynamic_property_t {
public:
    dynamic_property_t() = default;
    dynamic_property_t(const dynamic_property_t&) = default;
    dynamic_property_t(dynamic_property_t&&) = default;
    dynamic_property_t& operator=(const dynamic_property_t&) = default;
    dynamic_property_t& operator=(dynamic_property_t&&) = default;

    dynamic_property_t(const any_t& any)
    {
        m_value = any;
    }

    operator bool() const
    {
        return !m_value.empty();
    }

    template <typename T>
    dynamic_property_t& operator=(T&& val)
    {
        m_value = std::forward<T>(val);
        return *this;
    }

    template <typename T>
    const T as() const
    {
        return std::experimental::any_cast<const T>(m_value);
    }

    template <typename T>
    T as()
    {
        return std::experimental::any_cast<T>(m_value);
    }

    template <typename T>
    bool is() const
    {
        return std::experimental::any_cast<T>(&m_value);
    }

    any_t value() const
    {
        return m_value;
    }

private:
    any_t m_value;
};

template <typename T, std::size_t max_size = sizeof(double) * 2>
struct property_trait {
    static constexpr bool is_trivial = std::is_trivial<T>::value && sizeof(T) <= max_size;

    using access_type = typename std::conditional<is_trivial, T, const T&>::type;
};

template <typename T, typename AccessType = typename property_trait<T>::access_type>
class property_t {
public:
    using property_type = T;
    using access_type = AccessType;

    property_t()
    {
    }
    property_t(AccessType v) : m_value(v)
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
    property_t& operator=(V&& v)
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
