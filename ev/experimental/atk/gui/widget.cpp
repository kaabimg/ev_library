#include "widget.hpp"
#include "../core/object.hpp"

#include <qlayout.h>
#include <qvariant.h>

namespace ev {
namespace atk {
ATK_DECLARE_ENUM_QHASH(widget_attribute_e)
}
}

using namespace ev::atk;

struct widget_t::impl_t {
    qhash<widget_attribute_e, qvariant> attributes;
    object_t* obj = nullptr;
};

widget_t::widget_t(qwidget* parent) : qwidget(parent), d(new impl_t)
{
}

object_t* widget_t::object() const
{
    return d->obj;
}

void widget_t::set_object(object_t* obj) const
{
    d->obj = obj;
}

qvariant widget_t::attribute(widget_attribute_e att) const
{
    return d->attributes.value(att);
}

void widget_t::set_attribute(widget_attribute_e att, const qvariant& val)
{
    d->attributes[att] = val;
    attribute_changed(att);
}

widget_t* widget_t::make_from(qwidget* content, qwidget* parent)
{
    widget_t* widget = new widget_t(parent);
    QHBoxLayout* layout= new QHBoxLayout(widget);
    layout->addWidget(content);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    return widget;
}
