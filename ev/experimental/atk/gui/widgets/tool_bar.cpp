#include "tool_bar.hpp"
#include "../main_window.hpp"
#include "../style.hpp"
#include <qpainter.h>
#include <qaction.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <qdebug.h>

using namespace ev::atk;

tool_bar_t::tool_bar_t(QWidget* parent) : QToolBar(parent)
{
    apply_style(this);

    setOrientation(Qt::Horizontal);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);

//    QPalette palette = m_label->palette();
//    palette.setColor(m_label->backgroundRole(), atk_main_window->window_palette().dark_gray.name());
//    palette.setColor(m_label->foregroundRole(), atk_main_window->window_palette().light.name());
//    m_label->setPalette(palette);

    addWidget(m_label)->setVisible(true);
    m_label->setVisible(true);
    addWidget(new QLineEdit("**************",this))->setVisible(true);
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
