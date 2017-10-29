#pragma once

#include "preprocessor.hpp"
#include "basic_types.hpp"
#include "property.hpp"

#include <vector>
#include <algorithm>
#include <functional>
#include <unordered_map>

namespace ev {
class object;
class event {
public:
    event(object* target, int32_t t) : m_target{target}, m_type{t}
    {
    }
    virtual ~event()
    {
    }
    object* target() const
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
    object* m_target   = nullptr;
    const int32_t m_type = 0;
};

class object : non_copyable {
public:
    inline object(object* parent = nullptr);
    inline virtual ~object();
    inline void set_parent(object* parent);

    inline std::string name()const;
    inline void set_name(const std::string &);

    inline virtual dynamic_property property(const std::string&) const;
    inline virtual void set_property(const std::string&, const any_t&);
    inline virtual std::vector<std::string> properties_names() const;

    inline virtual bool process_event(event*);

private:
    inline void add_child(object*);
    inline void remove_child(object*);

private:
    object* m_parent = nullptr;
    std::string m_name;
    std::vector<object*> m_children;
    std::unordered_map<std::string, dynamic_property> m_dynamic_properties;
};

object::object(object* parent)
{
    set_parent(parent);
}

object::~object()
{
    if (m_parent) m_parent->remove_child(this);
    ev_delete_all(m_children);
}

void object::set_parent(object* parent)
{
    if (m_parent) m_parent->remove_child(this);
    m_parent = parent;
    if (m_parent) m_parent->add_child(this);
}

std::string object::name() const
{
    return m_name;
}

void object::set_name(const std::string & name)
{
    m_name = name;
}

void object::add_child(object* c)
{
    m_children.push_back(c);
}

void object::remove_child(object* c)
{
    auto it = std::find(m_children.begin(), m_children.end(), c);
    if (it != m_children.end()) m_children.erase(it);
}

dynamic_property object::property(const std::string& name) const
{
    auto iter = m_dynamic_properties.find(name);
    if (iter != m_dynamic_properties.end()) return iter->second;
    return dynamic_property();
}
void object::set_property(const std::string& name, const any_t& value)
{
    m_dynamic_properties[name] = value;
}

std::vector<std::string> object::properties_names() const
{
    std::vector<std::string> names;

    for (auto& prop : m_dynamic_properties) names.push_back(prop.first);

    return names;
}

bool object::process_event(event*)
{
    return false;
}

}  // ev
