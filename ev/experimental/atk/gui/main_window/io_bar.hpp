#pragma once

#include "../../qtypes_fwd.hpp"
#include <ev/core/preprocessor.hpp>
#include <qwidget.h>
#include <qtoolbutton.h>

namespace ev {
namespace atk {

class widget_t;


class io_bar_t : public qwidget {
    Q_OBJECT
public:
    io_bar_t(QWidget* parent = 0);
    virtual ~io_bar_t();
    void add_widget(qwidget*);
    void add_pane(const qstring& text, widget_t*);

Q_SIGNALS:
    void show_request(widget_t*);
    void hide_request();

protected:
    void paintEvent(QPaintEvent* e);
protected Q_SLOTS:
    void on_button_toggled(bool);
    void animate_panes();
    void animate_pane();
    void animate_pane(widget_t*);

private:
    EV_IMPL(io_bar_t)
};

class io_bar_button_t : public QToolButton
{
    Q_OBJECT
public:
    io_bar_button_t(int id, QWidget* parent = 0);
    QSize sizeHint() const;

    void set_animated(bool animated);

protected:
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEvent* e);
    void leaveEvent(QEvent* e);
    float roundness();

private:
    qstring m_number;
    bool m_is_animated, m_hover;
};


}
}
