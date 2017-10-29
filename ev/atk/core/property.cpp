#include "property.hpp"
#include "../preprocessor.hpp"

#include <boost/variant.hpp>

#include <qhash.h>
#include <qmetaobject.h>
#include <qvariant.h>
#include <qvector.h>

#define _SLOT(a) qPrintable("1" + a)
#define _SIGNAL(a) qPrintable("2" + a)

namespace ev {
namespace atk {
inline uint qHash(property_attribute_e key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}

QByteArray notify_signal_signature(const qobject* object, const qbytearray& propertyName)
{
    const QMetaObject* mo = object->metaObject();
    int index             = mo->indexOfProperty(propertyName);
    if (index == -1) return "";
    QMetaProperty property = mo->property(index);
    if (property.hasNotifySignal())
        return _SIGNAL(QByteArray(property.notifySignal().methodSignature()));
    return "";
}
}
}

using namespace ev::atk;

struct property_t::impl {
    boost::variant<qvariant, std::pair<qobject*, qbytearray>> data;
    qhash<property_attribute_e, qvariant> attributes;

    property_type_e type;
    property_data_type_e data_type;
};

property_t::property_t(property_data_type_e type, qobject* obj, const qbytearray& name)
    : d(new impl)
{
    d->type      = property_type_e::object_property;
    d->data_type = type;
    d->data      = std::make_pair(obj, name);

    qbytearray nss = notify_signal_signature(obj, name);
    if (nss.size()) connect(obj, nss.constData(), this, SIGNAL(value_changed()));
}

property_t::property_t(property_data_type_e type, const qvariant& value) : d(new impl)
{
    d->type      = property_type_e::standalone;
    d->data_type = type;
    d->data      = value;
}

property_t::~property_t()
{
    delete d;
}

property_type_e property_t::type() const
{
    return d->type;
}

property_data_type_e property_t::data_type() const
{
    return d->data_type;
}

qvariant property_t::read() const
{
    if (d->type == property_type_e::standalone) return boost::get<qvariant>(d->data);

    const std::pair<qobject*, qbytearray>& data =
        boost::get<std::pair<qobject*, qbytearray>>(d->data);
    return data.first->property(data.second);
}

void property_t::write(const qvariant& val)
{
    if (d->type == property_type_e::standalone) {
        d->data = val;
        Q_EMIT value_changed();
    }
    std::pair<qobject*, qbytearray>& data = boost::get<std::pair<qobject*, qbytearray>>(d->data);
    data.first->setProperty(data.second, val);
}

qvariant property_t::attribute(property_attribute_e a) const
{
    return d->attributes.value(a);
}

void property_t::set_attribute(property_attribute_e a, const qvariant& val)
{
    d->attributes[a] = val;
    Q_EMIT attribute_changed(a);
}

///////////////////////////////////////
///////////////////////////////////////

struct property_group_t::impl {
    qvector<property_t*> properties;
    qvector<property_group_t*> subgroups;
    qhash<property_attribute_e, qvariant> attributes;
};

property_group_t::property_group_t(qobject* parent) : qobject{parent}, d(new impl)
{
}

property_group_t::~property_group_t()
{
    delete d;
}

void property_group_t::add_property(property_t* p)
{
    d->properties.push_back(p);
    p->setParent(this);
}

void property_group_t::remove_property(property_t* p)
{
    if (d->properties.removeAll(p)) {
        p->setParent(nullptr);
    }
}

void property_group_t::add_sbugroup(property_group_t* g)
{
    d->subgroups.push_back(g);
    g->setParent(this);
}

void property_group_t::remove_sbugroup(property_group_t* g)
{
    if (d->subgroups.removeAll(g)) {
        g->setParent(nullptr);
    }
}

const qvector<property_t*>& property_group_t::properties() const
{
    return d->properties;
}

const qvector<property_group_t*> property_group_t::subgroups() const
{
    return d->subgroups;
}

qvariant property_group_t::attribute(property_attribute_e a) const
{
    return d->attributes.value(a);
}

void property_group_t::set_attribute(property_attribute_e a, const qvariant& val)
{
    d->attributes[a] = val;
    Q_EMIT attribute_changed(a);
}
