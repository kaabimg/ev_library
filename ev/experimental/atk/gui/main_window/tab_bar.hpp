#pragma once

#include "../../qtypes_fwd.hpp"
#include <ev/core/preprocessor.hpp>
#include <qwidget.h>
#include <qtimer.h>

class QPropertyAnimation;

namespace ev {
namespace atk {

class widget_t;

class tab_element_t : public QWidget {
    Q_OBJECT
    Q_PROPERTY(float fade_value READ fade_value WRITE set_fade_value)

public:
    explicit tab_element_t(widget_t* widget, qwidget* parent = nullptr);
    ~tab_element_t();
    bool is_selected() const;

    widget_t* widget() const;
    float fade_value() const;
    QSize minimumSizeHint() const;
    void set_preferred_size(const QSize& size);

protected:
    void mousePressEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

protected:
    void fade_in();
    void fade_out();
    friend class tab_bar_t;

Q_SIGNALS:
    void selected(widget_t*, bool mouseEvent = false);

public Q_SLOTS:
    void set_fade_value(float arg);
    void set_selected(bool arg, bool mouse_event = false);

protected:
    QFont font_from_width(int w) const;
    QRect icon_rect() const;
    QRect text_rect() const;
    void adjust_rect(QRect& rect, int value = 4) const;

private:
    EV_IMPL(tab_element_t)
};

class tab_bar_t : public qwidget {
    Q_OBJECT
public:
    explicit tab_bar_t(qwidget* parent = 0);
    ~tab_bar_t();

    int current_index() const;
    qwidget* current_tab() const;
    int tab_count() const;


Q_SIGNALS:
    void show_request(widget_t*);

public Q_SLOTS:

    void add_tab(widget_t*, int position = -1);
    void remove_tab(widget_t*);
    void set_current_tab(widget_t*);
    void set_current_index(int);

    void add_widget(qwidget*);
    void insert_widget(qwidget*, int position);
    void remove_widget(qwidget*);

protected Q_SLOTS:
    void on_tab_selected(widget_t*, bool mouse_event);

protected:
    tab_element_t* create_tab_element(widget_t*);
    tab_element_t* current_tab_element() const;
    void remove_hover_indicator();
protected:
    void paintEvent(QPaintEvent*);


private:
    EV_IMPL(tab_bar_t)
};
}
}
