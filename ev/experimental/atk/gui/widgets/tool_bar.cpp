#include "tool_bar.hpp"
#include "../main_window.hpp"

#include <qpainter.h>
#include <qaction.h>
#include <qlabel.h>

#include <qdebug.h>

using namespace ev::atk;

tool_bar_t::tool_bar_t(QWidget* parent) : QToolBar(parent), m_label(nullptr)
{
    setStyleSheet(QString("QToolBar { background-color: %0;} "
                          "QToolButton{color: rgba(255,255,255,200);} "
                          "QToolButton:hover{background: rgba(255,255,255,40);}")
                      .arg(atk_main_window->window_palette().dark_gray.name()));
    setOrientation(Qt::Horizontal);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);

    QPalette palette = m_label->palette();
    palette.setColor(m_label->foregroundRole(), atk_main_window->window_palette().light.name());
    m_label->setPalette(palette);

    addWidget(m_label);
}

void tool_bar_t::set_label(const qstring& label)
{
    m_label->setText(label);
}

void tool_bar_t::set_actions(const qlist<qaction*>& actions)
{
    clear();
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
    painter.fillRect(rect(), atk_main_window->window_palette().dark_gray);
    painter.setPen(atk_main_window->window_palette().dark);
    painter.drawRect(rect());
    QToolBar::paintEvent(event);
}
