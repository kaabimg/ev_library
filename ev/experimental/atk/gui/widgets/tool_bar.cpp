#include "tool_bar.hpp"
#include "../main_window.hpp"
#include "../style.hpp"
#include <qpainter.h>
#include <qaction.h>
#include <qlabel.h>


using namespace ev::atk;

tool_bar_t::tool_bar_t(QWidget* parent) : QToolBar(parent)
{
    apply_style(this);

    setOrientation(Qt::Horizontal);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    QPalette palette = m_label->palette();
    palette.setColor(m_label->backgroundRole(), atk_window_palette.background_color());
    palette.setColor(m_label->foregroundRole(), atk_window_palette.text_color());
    m_label->setPalette(palette);
    m_label->setMinimumWidth(100);

    addWidget(m_label)->setVisible(true);

    addAction(atk_main_window->std_icon(standard_icon_e::left_indicator),"hide",[&](){this->hide();});
    addAction(atk_main_window->std_icon(standard_icon_e::right_indicator),"hide",[&](){this->hide();});
}

void tool_bar_t::set_label(const qstring& label)
{
    m_label->setText(label);
}

void tool_bar_t::set_actions(const qlist<qaction*>& actions)
{
    for (int i = 0; i < actions.size(); ++i) {
        QWidget* spacer = new QWidget(this);
        spacer->setMinimumWidth(10);
        addWidget(spacer);
        addAction(actions[i]);
    }
}
