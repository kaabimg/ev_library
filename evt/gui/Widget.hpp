#pragma once

#include <ev/core/pimpl.hpp>

#include <QWidget>
#include <QToolBar>

namespace evt {
class Style;

class Widget : public QWidget {
    Q_OBJECT
public:
    Widget(QWidget* parent = nullptr);

    ~Widget();

    void setContent(QWidget*);
    QWidget* content() const;
    QToolBar* toolbar() const;
    void applyStyle(const Style&);

    static Widget* fromContent(QWidget* content, QWidget* parent = nullptr);

public Q_SLOTS:
    void resetToolBar();

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
