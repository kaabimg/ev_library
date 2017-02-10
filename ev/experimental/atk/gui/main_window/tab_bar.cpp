#include "tab_bar.hpp"
#include "../widget.hpp"
#include "../main_window.hpp"
#include "../../preprocessor.hpp"

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
    float fade_value;
    QSize preferred_size;
};

tab_element_t::tab_element_t(widget_t* widget, qwidget* parent) : qwidget(parent), d(new impl_t)

{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);

    d->widget     = widget;
    d->selected   = false;
    d->fade_value = 0;
    d->animator   = new QPropertyAnimation(this, "fade_value", this);
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

float tab_element_t::fade_value() const
{
    return d->fade_value;
}

QSize tab_element_t::minimumSizeHint() const
{
    return d->preferred_size;
}

void tab_element_t::set_preferred_size(const QSize& size)
{
    d->preferred_size = size;
}

QRect tab_element_t::icon_rect() const
{
    QRect rect = QRect(this->rect().topLeft(), QSize(width(), height() * 0.7));
    int adjust = rect.width() - rect.height();
    adjust /= 2;
    rect.adjust(adjust, 0, -adjust, 0);
    adjust_rect(rect, 2);
    return rect;
}

QRect tab_element_t::text_rect() const
{
    QRect rect(0, 0, width(), height() * 0.3);
    rect.translate(0, height() * 0.7);
    return rect;
}

void tab_element_t::adjust_rect(QRect& rect, int value) const
{
    rect.adjust(value, value, -value, -value);
}

void tab_element_t::set_selected(bool arg, bool mouse_event)
{
    if (d->selected != arg) {
        d->selected = arg;
        update();
        if (d->selected) Q_EMIT selected(d->widget, mouse_event);
    }
}

void tab_element_t::set_fade_value(float arg)
{
    d->fade_value = arg;
    update();
}

QFont tab_element_t::font_from_width(int w) const
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

void tab_element_t::fade_in()
{
    d->animator->stop();
    d->animator->setDuration(80);
    d->animator->setEndValue(255);
    d->animator->start();
}

void tab_element_t::fade_out()
{
    d->animator->stop();
    d->animator->setDuration(160);
    d->animator->setEndValue(0);
    d->animator->start();
}

void tab_element_t::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRect background_rect = rect();

    if (d->selected) {
        painter.save();
        painter.fillRect(background_rect, atk_window_palette.selected_color());
        painter.restore();
    }

    else {
        int value      = int(fade_value());
        auto col = atk_window_palette.highlight_color();
        col.setAlpha(value);
        painter.fillRect(background_rect, col);
    }

    auto icon = d->widget->icon();
    if (!icon.isNull()) {
        QRect iconRect = this->icon_rect();
        QPixmap pixmap = icon.pixmap(iconRect.size());
        painter.drawPixmap(iconRect, pixmap);
    }

    QFont font;
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(atk_window_palette.text_color());

    painter.drawText(text_rect(), Qt::AlignCenter, d->widget->title());
}

void tab_element_t::enterEvent(QEvent*)
{
    fade_in();
}

void tab_element_t::leaveEvent(QEvent*)
{
    fade_out();
}

void tab_element_t::mousePressEvent(QMouseEvent* e)
{
    e->accept();
    set_selected(true, true);
}

//////////////////////////////////////////

struct tab_bar_t::impl_t {
    Qt::Orientation orientation;
    QBoxLayout *main_layout, *tabs_layout, *widgets_layout, *spacer_layout;
    QWidget* spacer;
    QList<tab_element_t*> tabs;
    QSize tab_size;
};

tab_bar_t::tab_bar_t(qwidget* parent) : qwidget(parent), d(new impl_t)
{
    d->main_layout    = new QVBoxLayout;
    d->tabs_layout    = new QVBoxLayout;
    d->widgets_layout = new QVBoxLayout;
    d->spacer_layout  = new QVBoxLayout;

    d->spacer = new QWidget(this);
    d->spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->spacer_layout->addWidget(d->spacer);

    d->main_layout->setSpacing(0);
    d->main_layout->setMargin(0);
    d->main_layout->setContentsMargins(0, 0, 0, 0);

    d->tabs_layout->setSpacing(0);
    d->tabs_layout->setMargin(0);
    d->tabs_layout->setContentsMargins(0, 0, 0, 0);

    d->widgets_layout->setSpacing(0);
    d->widgets_layout->setMargin(0);
    d->widgets_layout->setContentsMargins(0, 0, 0, 0);

    d->spacer_layout->setSpacing(0);
    d->spacer_layout->setMargin(0);
    d->spacer_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(d->main_layout);

    d->tab_size = QSize(80, 55);

    d->main_layout->addLayout(d->tabs_layout);
    d->main_layout->addLayout(d->widgets_layout);
    d->main_layout->addLayout(d->spacer_layout);

    setMaximumWidth(80);
}

tab_bar_t::~tab_bar_t()
{
    delete d;
}

int tab_bar_t::current_index() const
{
    tab_element_t* current = current_tab_element();
    if (current) return d->tabs_layout->indexOf(current);
    return -1;
}

qwidget* tab_bar_t::current_tab() const
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->is_selected()) {
            return d->tabs[i]->widget();
        }
    }
    return nullptr;
}

tab_element_t* tab_bar_t::current_tab_element() const
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->is_selected()) {
            return d->tabs[i];
        }
    }
    return nullptr;
}

int tab_bar_t::tab_count() const
{
    return d->tabs.size();
}

tab_element_t* tab_bar_t::create_tab_element(widget_t* widget)
{
    tab_element_t* tab_element = new tab_element_t(widget, this);

    d->tabs << tab_element;
    tab_element->set_preferred_size(d->tab_size);
    connect(ATK_SIGNAL(tab_element, selected), ATK_SLOT(this, on_tab_selected));
    return tab_element;
}

void tab_bar_t::add_tab(widget_t* widget, int position)
{
    tab_element_t* tab_element = create_tab_element(widget);
    if (position == -1)
        d->tabs_layout->addWidget(tab_element);
    else
        d->tabs_layout->insertWidget(position, tab_element);
}

void tab_bar_t::remove_tab(widget_t* widget)
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->widget() == widget) {
            tab_element_t* tab = d->tabs.takeAt(i);
            d->tabs_layout->removeWidget(tab);
            delete tab;
            return;
        }
    }
}

void tab_bar_t::add_widget(QWidget* widget)
{
    d->widgets_layout->addWidget(widget);
}

void tab_bar_t::insert_widget(QWidget* widget, int position)
{
    d->widgets_layout->insertWidget(position, widget);
}

void tab_bar_t::remove_widget(QWidget* widget)
{
    d->widgets_layout->removeWidget(widget);
}

void tab_bar_t::set_current_tab(widget_t* widget)
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->widget() == widget) {
            d->tabs[i]->set_selected(true);
            return;
        }
    }
}

void tab_bar_t::remove_hover_indicator()
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        d->tabs[i]->fade_out();
    }
}

void tab_bar_t::set_current_index(int index)
{
    if (index > -1 && index < tab_count()) {
        set_current_tab(
            qobject_cast<tab_element_t*>(d->tabs_layout->itemAt(index)->widget())->widget());
        remove_hover_indicator();
    }
}

void tab_bar_t::on_tab_selected(widget_t* widget, bool mouse_event)
{
    for (int i = 0; i < d->tabs.size(); ++i) {
        if (d->tabs[i]->widget() != widget) {
            d->tabs[i]->set_selected(false);
        }
    }
    if (mouse_event) Q_EMIT show_request(widget);
}

void tab_bar_t::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect rect = this->rect();
    painter.fillRect(rect, atk_window_palette.dark_gray);
    QWidget::paintEvent(event);
}
