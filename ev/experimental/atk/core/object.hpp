#pragma once

#include "../preprocessor.hpp"
#include "../attribute_interface.hpp"
#include "../qtypes_fwd.hpp"

#include <ev/core/preprocessor.hpp>

#include <qobject.h>
#include <qvector.h>

namespace ev {
namespace atk {

class object_t;

using object_list_t = qvector<object_t*>;

enum class object_attribute_e { label, description };

class object_t : public qobject, public attribute_interface<object_attribute_e> {
    Q_OBJECT
public:
    object_t(object_t* parent = nullptr);
    ~object_t();

    inline qstring name() const;
    inline void set_name(const qstring&);

    object_t* get_parent() const;
    inline int index_in_parent() const;

    void add(object_t*);
    void remove(object_t*);
    const object_list_t& children() const;

    qvariant attribute(object_attribute_e att) const;
    void set_attribute(object_attribute_e att, const qvariant& val);

    template <typename T>
    T* as();

    template <typename T>
    const T* as() const;

Q_SIGNALS:
    void children_changed();
    void attribute_changed(object_attribute_e att);

private:
    EV_IMPL(object_t)
};

qstring object_t::name() const
{
    return objectName();
}

void object_t::set_name(const qstring& name)
{
    setObjectName(name);
}

int object_t::index_in_parent() const
{
    if (const auto parent = get_parent())
        return parent->children().indexOf(const_cast<object_t*>(this));
    return 0;
}

template <typename T>
T* object_t::as()
{
    return qobject_cast<T*>(this);
}

template <typename T>
const T* object_t::as() const
{
    return qobject_cast<const T*>(this);
}
}
}
