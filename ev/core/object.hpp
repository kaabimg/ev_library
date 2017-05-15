#pragma once

#include "preprocessor.hpp"
#include "basic_types.hpp"
#include "property.hpp"

#include <vector>
#include <algorithm>
#include <functional>
#include <unordered_map>

namespace ev {
class object_t;
class event_t {
public:
    event_t(object_t* target, int32_t t) : m_target{target}, m_type{t}
    {
    }
    virtual ~event_t()
    {
    }
    object_t* target() const
    {
        return m_target;
    }
    int32_t type() const
    {
        return m_type;
    }
    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }

private:
    object_t* m_target   = nullptr;
    const int32_t m_type = 0;
};

class object_t : non_copyable_t {
public:
    inline object_t(object_t* parent = nullptr);
    inline virtual ~object_t();
    inline void set_parent(object_t* parent);

    inline std::string name()const;
    inline void set_name(const std::string &);

    inline virtual dynamic_property_t property(const std::string&) const;
    inline virtual void set_property(const std::string&, const any_t&);
    inline virtual std::vector<std::string> properties_names() const;

    inline virtual bool process_event(event_t*);

private:
    inline void add_child(object_t*);
    inline void remove_child(object_t*);

private:
    object_t* m_parent = nullptr;
    std::string m_name;
    std::vector<object_t*> m_children;
    std::unordered_map<std::string, dynamic_property_t> m_dynamic_properties;
};

object_t::object_t(object_t* parent)
{
    set_parent(parent);
}

object_t::~object_t()
{
    if (m_parent) m_parent->remove_child(this);
    ev_delete_all(m_children);
}

void object_t::set_parent(object_t* parent)
{
    if (m_parent) m_parent->remove_child(this);
    m_parent = parent;
    if (m_parent) m_parent->add_child(this);
}

std::string object_t::name() const
{
    return m_name;
}

void object_t::set_name(const std::string & name)
{
    m_name = name;
}

void object_t::add_child(object_t* c)
{
    m_children.push_back(c);
}

void object_t::remove_child(object_t* c)
{
    auto it = std::find(m_children.begin(), m_children.end(), c);
    if (it != m_children.end()) m_children.erase(it);
}

dynamic_property_t object_t::property(const std::string& name) const
{
    auto iter = m_dynamic_properties.find(name);
    if (iter != m_dynamic_properties.end()) return iter->second;
    return dynamic_property_t();
}
void object_t::set_property(const std::string& name, const any_t& value)
{
    m_dynamic_properties[name] = value;
}

std::vector<std::string> object_t::properties_names() const
{
    std::vector<std::string> names;

    for (auto& prop : m_dynamic_properties) names.push_back(prop.first);

    return names;
}

bool object_t::process_event(event_t*)
{
    return false;
}

}  // ev
