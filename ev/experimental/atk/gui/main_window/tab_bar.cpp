#include "tab_bar.hpp"
#include "../widget.hpp"
#include "../main_window.hpp"

#include <qboxlayout.h>
#include <qevent.h>
#include <qpainter.h>
#include <qpropertyanimation.h>
#include <qdebug.h>

using namespace ev::atk;

struct tab_element_t::impl_t {
    widget_t* widget;
    QPropertyAnimation* animator;
    bool selected;
    float fadeValue;
    QSize prefferedSize;
};

tab_element_t::tab_element_t(widget_t* widget, qwidget* parent) : qwidget(parent), d(new impl_t)

{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);

    d->widget    = widget;
    d->selected  = false;
    d->fadeValue = 0;
    d->animator  = new QPropertyAnimation(this, "fadeValue", this);
}

tab_element_t::~tab_element_t()
{
    delete d;
}

bool tab_element_t::is_selected() const
{
    return d->selected;
}

widget_t* tab_element_t::widget() const
{
    return d->widget;
}

float tab_element_t::fadeValue() const
{
    return d->fadeValue;
}

QSize tab_element_t::minimumSizeHint() const
{
    return d->prefferedSize;
}

void tab_element_t::setPrefferedSize(const QSize& size)
{
    d->prefferedSize = size;
}

QRect tab_element_t::iconRect() const
{
    QRect rect = QRect(this->rect().topLeft(),QSize(width(),height()*0.7));
    int adjust = rect.width()-rect.height();
    adjust/=2;
    rect.adjust(adjust,0,-adjust,0);
    adjustRect(rect,2);
    return rect;
}

QRect tab_element_t::textRect() const
{
    QRect rect(0, 0, width(), height() * 0.3);
    rect.translate(0, height() * 0.7);
    return rect;
}

void tab_element_t::adjustRect(QRect& rect, int value) const
{
    rect.adjust(value, value, -value, -value);
}

void tab_element_t::setSelected(bool arg, bool mouseEvent)
{
    if (d->selected != arg) {
        d->selected = arg;
        update();
        if (d->selected) Q_EMIT selected(d->widget, mouseEvent);
    }
}

void tab_element_t::setFadeValue(float arg)
{
    d->fadeValue = arg;
    update();
}

QFont tab_element_t::fontFromWidth(int w) const
{
    QFont font = this->font();
    if (d->selected) font.setBold(true);
    for (int i = font.pointSize(); i > 5; --i) {
        QFontMetrics metrics(font);
        if (metrics.width(d->widget->title()) <= w) return font;
        font.setPointSize(i - 1);
    }

    return font;
}

void tab_element_t::fadeIn()
{
    d->animator->stop();
    d->animator->setDuration(80);
    d->animator->setEndValue(40);
    d->animator->start();
}

void tab_element_t::fadeOut()
{
    d->animator->stop();
    d->animator->setDuration(160);
    d->animator->setEndValue(0);
    d->animator->start();
}

void tab_element_t::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRect backgroundRect = rect();

    if (d->selected) {
        painter.save();
        painter.fillRect(backgroundRect, atk_window_palette.dark_gray.darker());
        painter.restore();
    }

    if (!d->selected) {
        int value      = int(fadeValue());
        QColor bgColor = atk_main_window->window_palette().light_gray;
        bgColor.setAlpha(value);
        painter.fillRect(backgroundRect, bgColor);
        painter.setPen(QColor(255, 255, 255, value).darker());
        painter.drawLine(backgroundRect.topLeft(), backgroundRect.topRight());
        painter.drawLine(backgroundRect.bottomLeft(), backgroundRect.bottomRight());
    }

    auto icon = d->widget->icon();
    if(!icon.isNull())
    {
        QRect iconRect = this->iconRect();
        QPixmap pixmap = icon.pixmap(iconRect.size());
        painter.drawPixmap(iconRect, pixmap);
    }

    painter.setPen(atk_main_window->window_palette().light);

    QFont font;
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(atk_main_window->window_palette().light);

    painter.drawText(textRect(), Qt::AlignCenter, d->widget->title());
}

void tab_element_t::enterEvent(QEvent*)
{
    fadeIn();
}

void tab_element_t::leaveEvent(QEvent*)
{
    fadeOut();
}

void tab_element_t::mousePressEvent(QMouseEvent* e)
{
    e->accept();
    setSelected(true, true);
}

//////////////////////////////////////////

struct tab_bar_t::impl_t {
    Qt::Orientation orientation;
    QBoxLayout *mainLayout, *tabsLayout, *widgetsLayout, *spacerLayout;
    QWidget* spacer;
    QList<tab_element_t*> tabs;
    QColor selectionColor;
    QColor outlineColor;
    QSize tabSize;
    int maximumSize;
};

tab_bar_t::tab_bar_t(qwidget* parent) : qwidget(parent), d(new impl_t)
{
    d->maximumSize   = -1;
    d->mainLayout    = new QVBoxLayout;
    d->tabsLayout    = new QVBoxLayout;
    d->widgetsLayout = new QVBoxLayout;
    d->spacerLayout  = new QVBoxLayout;

    d->spacer = new QWidget(this);
    d->spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->spacerLayout->addWidget(d->spacer);

    d->mainLayout->setSpacing(0);
    d->mainLayout->setMargin(0);
    d->mainLayout->setContentsMargins(0, 0, 0, 0);

    d->tabsLayout->setSpacing(0);
    d->tabsLayout->setMargin(0);
    d->tabsLayout->setContentsMargins(0, 0, 0, 0);

    d->widgetsLayout->setSpacing(0);
    d->widgetsLayout->setMargin(0);
    d->widgetsLayout->setContentsMargins(0, 0, 0, 0);

    d->spacerLayout->setSpacing(0);
    d->spacerLayout->setMargin(0);
    d->spacerLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(d->mainLayout);

    d->tabSize = QSize(80, 55);

    d->mainLayout->addLayout(d->tabsLayout);
    d->mainLayout->addLayout(d->widgetsLayout);
    d->mainLayout->addLayout(d->spacerLayout);

    setMaximumWidth(80);
}

tab_bar_t::~tab_bar_t()
{
    delete d;
}

int tab_bar_t::currentIndex() const
{
    tab_element_t* current = currentTabElement();
    if (current) return d->tabsLayout->indexOf(current);
    return -1;
}

qwidget* tab_bar_t::currentTab() const
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->is_selected()) {
            return d->tabs[i]->widget();
        }
    }
    return nullptr;
}

tab_element_t* tab_bar_t::currentTabElement() const
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->is_selected()) {
            return d->tabs[i];
        }
    }
    return nullptr;
}

int tab_bar_t::tabsCount() const
{
    return d->tabs.size();
}

tab_element_t* tab_bar_t::createTabElement(widget_t* widget)
{
    tab_element_t* tabElement = new tab_element_t(widget, this);

    d->tabs << tabElement;
    tabElement->setPrefferedSize(d->tabSize);
    connect(tabElement, SIGNAL(selected(widget_t*, bool)), this,
            SLOT(onTabSelected(widget_t*, bool)));
    return tabElement;
}

void tab_bar_t::insertTab(widget_t* widget, int position)
{
    tab_element_t* tabElement = createTabElement(widget);
    if(position == -1)
        d->tabsLayout->addWidget(tabElement);
    else
        d->tabsLayout->insertWidget(position, tabElement);
}

void tab_bar_t::removeTab(widget_t* widget)
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->widget() == widget) {
            tab_element_t* tab = d->tabs.takeAt(i);
            d->tabsLayout->removeWidget(tab);
            delete tab;
            return;
        }
    }
}

void tab_bar_t::addWidget(QWidget* widget)
{
    d->widgetsLayout->addWidget(widget);
}

void tab_bar_t::insertWidget(QWidget* widget, int position)
{
    d->widgetsLayout->insertWidget(position, widget);
}

void tab_bar_t::removeWidget(QWidget* widget)
{
    d->widgetsLayout->removeWidget(widget);
}

void tab_bar_t::setCurrentTab(widget_t* widget)
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->widget() == widget) {
            d->tabs[i]->setSelected(true);
            return;
        }
    }
}

void tab_bar_t::removeHoverIndicator()
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        d->tabs[i]->fadeOut();
    }
}

void tab_bar_t::setCurrentIndex(int index)
{
    if (index > -1 && index < tabsCount()) {
        setCurrentTab(
            qobject_cast<tab_element_t*>(d->tabsLayout->itemAt(index)->widget())->widget());
        removeHoverIndicator();
    }
}

QSize tab_bar_t::tabSize() const
{
    return d->tabSize;
}

int tab_bar_t::maximumSize() const
{
    return d->maximumSize;
}

void tab_bar_t::setMaximumSize(int arg)
{
    d->maximumSize = arg;
}

void tab_bar_t::onTabSelected(widget_t* widget, bool mouseEvent)
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->widget() != widget) {
            d->tabs[i]->setSelected(false);
        }
    }
    if(mouseEvent)
    Q_EMIT showRequest(widget);
}

void tab_bar_t::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect rect = this->rect();
    painter.fillRect(rect, atk_window_palette.dark_gray);
    QWidget::paintEvent(event);
}
