#pragma once

#include "../interfaces/attribute_interface.hpp"

#include <ev/core/preprocessor.hpp>

#include <qwidget.h>
#include <qicon.h>

namespace ev {
namespace atk {

class object_t;

enum class widget_attribute_e {

};

class widget_t : public qwidget, public attribute_interface<widget_attribute_e> {
    Q_OBJECT
public:
    widget_t(qwidget* parent = nullptr);
    static widget_t* make_from(qwidget*, qwidget* parent = nullptr);

    object_t* object() const;
    void set_object(object_t*) const;

    void enable_toolbar(bool);

    void set_content(qwidget*);

    qvariant attribute(widget_attribute_e att) const;
    void set_attribute(widget_attribute_e att, const qvariant& val);

    inline qstring title() const;
    void set_title(const qstring& title);

    inline qicon icon() const;
    inline void set_icon(const qicon& icon);

Q_SIGNALS:

    void attribute_changed(widget_attribute_e att);
    void show_request();
    void hide_request();

private:
    EV_IMPL(widget_t)
};

qstring widget_t::title() const
{
    return windowTitle();
}

qicon widget_t::icon() const
{
    return windowIcon();
}

void widget_t::set_icon(const qicon &icon)
{
    setWindowIcon(icon);
}
}
}
