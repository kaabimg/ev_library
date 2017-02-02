#include "tool_bar.hpp"
#include <qpainter.h>
#include <qaction.h>
#include <qlabel.h>
#include "../main_window.hpp"

using namespace ev::atk;

tool_bar_t::tool_bar_t(int label_width, QWidget* parent)
    : QToolBar(parent), m_label(NULL), m_current_widget(NULL)
{
    setStyleSheet(QString("QToolBar { background-color: %0;} "
                          "QToolButton{color: rgba(255,255,255,200);} "
                          "QToolButton:hover{background: rgba(255,255,255,40);}")
                      .arg(atk_main_window->window_palette().dark_gray.name()));
    setOrientation(Qt::Horizontal);

    if (label_width) {
        m_label = new QLabel(this);
        m_label->setFixedWidth(label_width);
        m_label->setAlignment(Qt::AlignCenter);

        QPalette palette = m_label->palette();
        palette.setColor(m_label->foregroundRole(), atk_main_window->window_palette().light.name());
        m_label->setPalette(palette);

        addWidget(m_label);
    }
}

void tool_bar_t::set_current_widget(QWidget* widget)
{
    if (m_current_widget == widget) return;

    if (m_label && m_current_widget) {
        disconnect(m_current_widget, SIGNAL(windowTitleChanged(QString)), m_label,
                   SLOT(setText(QString)));
    }

    if (m_current_widget) {
        disconnect(m_current_widget, SIGNAL(destroyed(QObject*)), this, SLOT(reset()));
    }

    reset();

    m_current_widget = widget;

    if (!m_current_widget) {
        return;
    }

    connect(m_current_widget, SIGNAL(destroyed(QObject*)), this, SLOT(reset()));

    addActions(m_current_widget->actions());
    if (m_label) {
        m_label->setText(m_current_widget->windowTitle());
        connect(m_current_widget, SIGNAL(windowTitleChanged(QString)), m_label,
                SLOT(setText(QString)));
    }

    Q_EMIT current_widget_changed(m_current_widget);
}
void tool_bar_t::reset()
{
    QList<QAction*> acts = actions();
    for (int i = 1; i < acts.size(); ++i) {  // bypass the label
        removeAction(acts[i]);
        if (acts[i]->isSeparator() && acts[i]->parent() == this) {
            acts[i]->deleteLater();
        }
    }

    m_current_widget = nullptr;
}

void tool_bar_t::set_actions(const QList<QAction*>& actions)
{
    for (int i = 0; i < actions.size(); ++i) {
        QWidget* spacer = new QWidget(this);
        spacer->setMinimumWidth(10);
        spacer->setProperty("spacer", true);
        addWidget(spacer);
        addAction(actions[i]);
    }
}

void tool_bar_t::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(),atk_main_window->window_palette().dark_gray);
    painter.setPen(atk_main_window->window_palette().dark);
    painter.drawRect(rect());
    QToolBar::paintEvent(event);
}
