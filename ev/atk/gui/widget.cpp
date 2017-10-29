#include "widget.hpp"
#include "widgets/tool_bar.hpp"
#include "../core/object.hpp"

#include <qlayout.h>
#include <qvariant.h>

namespace ev {
namespace atk {
ATK_DECLARE_ENUM_QHASH(widget_attribute_e)
}
}

using namespace ev::atk;

struct widget_t::impl {
    qhash<widget_attribute_e, qvariant> attributes;
    QVBoxLayout* layout;
    tool_bar_t* tool_bar;

    object_t* obj = nullptr;
};

widget_t::widget_t(qwidget* parent) : qwidget(parent), d(new impl)
{
    d->layout   = new QVBoxLayout(this);
    d->tool_bar = new tool_bar_t(this);

    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);

    d->layout->addWidget(d->tool_bar);
}

object_t* widget_t::object() const
{
    return d->obj;
}

void widget_t::set_object(object_t* obj) const
{
    d->obj = obj;
}

void widget_t::enable_toolbar(bool enabled)
{
    d->tool_bar->setVisible(enabled);
}

void widget_t::set_content(qwidget* widget)
{
    widget->setParent(this);
    d->layout->addWidget(widget);
    d->tool_bar->set_actions(widget->actions());
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
    widget->set_content(content);
    return widget;
}

void widget_t::set_title(const qstring& title)
{
    setWindowTitle(title);
    d->tool_bar->set_label(title);
}
