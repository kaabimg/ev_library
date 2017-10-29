#include "object.hpp"

#include <qvector.h>
#include <qhash.h>
#include <qvariant.h>

namespace ev {
namespace atk {
ATK_DECLARE_ENUM_QHASH(object_attribute_e)
}
}

using namespace ev::atk;

struct object_t::impl {
    qvector<object_t*> chilren;
    qhash<object_attribute_e, qvariant> attributes;
    ev::flags<object_state_f> state;
};

object_t::object_t(object_t* parent) : qobject(parent), d(new impl)
{
    if (parent) parent->add(this);
}

object_t::~object_t()
{
    delete d;
}

void object_t::set_state(object_state_f s, bool enabled)
{
    if(enabled)
        d->state.enable(s);
    else d->state.clear(s);
    Q_EMIT state_changed();
}

ev::flags<object_state_f> object_t::state() const
{
    return d->state;
}

object_t* object_t::get_parent() const
{
    if (qobject* p = parent()) return qobject_cast<object_t*>(p);
    return nullptr;
}

void object_t::add(object_t* obj)
{
    if (d->chilren.contains(obj)) return;

    obj->setParent(this);
    d->chilren.push_back(obj);
    Q_EMIT children_changed();
}

void object_t::remove(object_t* obj)
{
    if (d->chilren.removeAll(obj)) {
        obj->setParent(nullptr);
        Q_EMIT children_changed();
    }
}

const object_list_t& object_t::children() const
{
    return d->chilren;
}

qvariant object_t::attribute(object_attribute_e att) const
{
    return d->attributes.value(att);
}

void object_t::set_attribute(object_attribute_e att, const qvariant& val)
{
    d->attributes[att] = val;
    Q_EMIT attribute_changed(att);
}
