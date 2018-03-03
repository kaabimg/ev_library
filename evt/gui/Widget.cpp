#include "Widget.hpp"
#include "Style.hpp"
#include "../style/stylesheets/MainWindow.hpp"
#include <QToolBar>
#include <QVBoxLayout>

using namespace evt;

struct Widget::Impl {
    QVBoxLayout* layout;
    QToolBar* toolbar;
    QWidget* content = nullptr;
};

Widget::Widget(QWidget* parent) : QWidget(parent)
{
    d->layout = new QVBoxLayout(this);
    d->toolbar = new QToolBar(this);

    d->layout->setMargin(0);
    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->setSpacing(0);

    d->layout->addWidget(d->toolbar);

    applyStyle(appStyle());
    monitorStyleChangeFor(this);
}

Widget::~Widget()
{
}

void Widget::setContent(QWidget* content)
{
    d->layout->addWidget(content);
    d->content = content;
    content->setParent(this);
    for (auto action : content->actions()) {
        d->toolbar->addAction(action);
    }
}

QWidget* Widget::content() const
{
    return d->content;
}

QToolBar* Widget::toolbar() const
{
    return d->toolbar;
}

Widget* Widget::fromContent(QWidget* content, QWidget* parent)
{
    auto w = new Widget(parent);
    w->setContent(content);
    return w;
}

void Widget::resetToolBar()
{
    d->toolbar->clear();
    for (auto action : d->content->actions()) {
        d->toolbar->addAction(action);
    }
}

void Widget::applyStyle(const Style& style)
{
    d->toolbar->setStyleSheet(style.adaptStyleSheet(toolBarStyleSheet));
}
