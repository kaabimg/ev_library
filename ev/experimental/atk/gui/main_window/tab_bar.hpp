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
    Q_PROPERTY(float fadeValue READ fadeValue WRITE setFadeValue)

public:
    explicit tab_element_t(widget_t* widget, qwidget* parent = nullptr);
    ~tab_element_t();
    bool is_selected() const;

    widget_t* widget() const;
    float fadeValue() const;
    QSize minimumSizeHint() const;
    void setPrefferedSize(const QSize& size);

protected:
    void mousePressEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

protected:
    void fadeIn();
    void fadeOut();
    friend class tab_bar_t;

Q_SIGNALS:
    void selected(widget_t*, bool mouseEvent = false);

public Q_SLOTS:
    void setFadeValue(float arg);
    void setSelected(bool arg, bool mouseEvent = false);

protected:
    QFont fontFromWidth(int w) const;
    QRect iconRect() const;
    QRect textRect() const;
    void adjustRect(QRect& rect, int value = 4) const;

private:
    EV_IMPL(tab_element_t)
};

class tab_bar_t : public qwidget {
    Q_OBJECT
public:
    explicit tab_bar_t(qwidget* parent = 0);
    ~tab_bar_t();

    int currentIndex() const;
    qwidget* currentTab() const;
    int tabsCount() const;

    QSize tabSize() const;
    int maximumSize() const;

Q_SIGNALS:
    void showRequest(qwidget*);

public Q_SLOTS:

    void insertTab(widget_t*, int position = -1);
    void removeTab(widget_t*);
    void setCurrentTab(widget_t*);
    void setCurrentIndex(int);

    void addWidget(qwidget*);
    void insertWidget(qwidget*, int position);
    void removeWidget(qwidget*);

    void setMaximumSize(int arg);

protected Q_SLOTS:
    void onTabSelected(widget_t*, bool mouseEvent);

protected:
    tab_element_t* createTabElement(widget_t*);
    tab_element_t* currentTabElement() const;

    void paintEvent(QPaintEvent*);
    void removeHoverIndicator();

private:
    EV_IMPL(tab_bar_t)
};
}
}
