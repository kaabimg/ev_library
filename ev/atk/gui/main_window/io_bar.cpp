#include "io_bar.hpp"
#include "../widget.hpp"
#include "../main_window.hpp"
#include <qlayout.h>
#include <qaction.h>
#include <qevent.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qapplication.h>

using namespace ev::atk;

io_bar_button_t::io_bar_button_t(int number, QWidget* parent)
    : QToolButton(parent), m_number(QString::number(number)), m_is_animated(false), m_hover(false)
{
    setFocusPolicy(Qt::NoFocus);
    setCheckable(true);
    setMouseTracking(true);
}

QSize io_bar_button_t::sizeHint() const
{
    ensurePolished();
    QSize s = fontMetrics().size(Qt::TextSingleLine, text());
    s *= 1.1;
    s.rwidth() += 26;
    s.rheight() += 4;
    return s.expandedTo(QApplication::globalStrut());
}

void io_bar_button_t::set_animated(bool animated)
{
    m_is_animated = animated;
    update();
}

float io_bar_button_t::roundness()
{
    static const float factor = 5;
    return qMin(30.0f, qMax(height() / factor, width() / factor));
}

void io_bar_button_t::paintEvent(QPaintEvent*)
{
    const QFontMetrics fm = fontMetrics();
    const int baseLine    = (height() - fm.height() + 1) / 2 + fm.ascent();
    const int numberWidth = fm.width(m_number);
    QPainter p(this);
    p.setRenderHint(QPainter::HighQualityAntialiasing, true);
    QPainterPath path;
    path.addRoundRect(rect(), roundness());
    if (m_is_animated) {
        p.fillPath(path, atk_main_window->window_palette().secondary);
    }
    else {
        if (isChecked())
            p.fillPath(path, atk_main_window->window_palette().selected_color());
        else if (m_hover) {
            p.fillPath(path, atk_main_window->window_palette().highlight_color());
        }
    }
    QFont f = font();
    p.setPen(atk_main_window->window_palette().text_color());

    p.setFont(f);
    p.drawText((20 - numberWidth) / 2, baseLine, m_number);
    int leftPart = 22;
    p.drawText(leftPart, baseLine, fm.elidedText(text(), Qt::ElideRight, width() - leftPart - 1));
}

void io_bar_button_t::enterEvent(QEvent* e)
{
    m_hover = true;
    QToolButton::enterEvent(e);
}

void io_bar_button_t::leaveEvent(QEvent* e)
{
    m_hover = false;
    QToolButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////

struct io_bar_t::impl {
    QHBoxLayout* layout;
    qvector<io_bar_button_t*> buttons;
    qvector<widget_t*> panes;
    QTimer animation_timer;
    int width;
    int current_selection_index;
    bool animation_cycle;
};

io_bar_t::io_bar_t(qwidget* parent) : qwidget(parent), d(new impl)
{

    setFixedHeight(atk_main_window->window_sizes().io_pane_height);

    d->current_selection_index = -1;

    d->layout = new QHBoxLayout;
    d->layout->setContentsMargins(3, 3, 3, 3);
    d->layout->setSpacing(10);
    d->layout->setDirection(QBoxLayout::LeftToRight);
    d->layout->setAlignment(Qt::AlignCenter);
    setLayout(d->layout);
    d->width = 0;

    d->animation_cycle = false;
    d->animation_timer.setInterval(800);
    connect(&d->animation_timer, SIGNAL(timeout()), this, SLOT(animate_panes()));
}

io_bar_t::~io_bar_t()
{
    delete d;
}

void io_bar_t::add_widget(qwidget* w)
{
    d->layout->addWidget(w);
    w->setParent(this);
}

void io_bar_t::add_pane(const qstring& text, widget_t* pane)
{
    io_bar_button_t* button = new io_bar_button_t(d->buttons.size() + 1, this);
    button->setText(text);
    d->layout->addWidget(button);
    d->buttons << button;
    d->panes << pane;
    connect(button, SIGNAL(toggled(bool)), this, SLOT(on_button_toggled(bool)));
    connect(pane, SIGNAL(show_request()), this, SLOT(animate_pane()));
}

void io_bar_t::on_button_toggled(bool toggled)
{
    io_bar_button_t* the_button = qobject_cast<io_bar_button_t*>(sender());
    if (toggled) {
        if (d->current_selection_index != -1) {
            io_bar_button_t* button = d->buttons[d->current_selection_index];
            button->blockSignals(true);
            button->setChecked(false);
            button->blockSignals(false);
        }
        d->current_selection_index = d->buttons.indexOf(the_button);
        widget_t* pane             = d->panes[d->current_selection_index];
        Q_EMIT show_request(pane);
    }
    else {
        d->current_selection_index = -1;
        Q_EMIT hide_request();
    }
}

void io_bar_t::animate_panes()
{
    d->animation_cycle = !d->animation_cycle;
    for (int i = 0; i < d->buttons.size(); ++i) {
        d->buttons[i]->set_animated(d->animation_cycle);
    }
}

void io_bar_t::animate_pane()
{
    widget_t* pane = qobject_cast<widget_t*>(sender());
    animate_pane(pane);
}

void io_bar_t::animate_pane(widget_t* pane)
{
    int index               = d->panes.indexOf(pane);
    io_bar_button_t* button = d->buttons.at(index);

    if (button->isChecked())  // pane is visible
        return;

    if (!d->buttons.contains(button)) d->buttons.append(button);

    if (!d->animation_timer.isActive()) d->animation_timer.start();
}

void io_bar_t::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), atk_main_window->window_palette().background_color());
    painter.setPen(atk_main_window->window_palette().dark);
    painter.drawRect(rect());
    QWidget::paintEvent(event);
}
