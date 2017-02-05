#include "message_pane.hpp"
#include "../main_window.hpp"
#include <qheaderview.h>

using namespace ev::atk;

message_pane_t::message_pane_t(qwidget* parent) : QTableWidget(parent)
{
    setRowCount(0);
    setColumnCount(2);
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
}

message_pane_t::~message_pane_t()
{
}

void message_pane_t::add_info(const qstring& msg)
{
    add_message(atk_main_window->std_icon(standard_icon_e::information),msg);
}

void message_pane_t::add_warning(const qstring& msg)
{
    add_message(atk_main_window->std_icon(standard_icon_e::warning),msg);
}

void message_pane_t::add_error(const qstring& msg)
{
    add_message(atk_main_window->std_icon(standard_icon_e::error),msg);

}

void message_pane_t::add_message(const qicon & icon, const qstring& msg)
{
    QTableWidgetItem* severity = new QTableWidgetItem();
    severity->setIcon(icon);
    QTableWidgetItem* text     = new QTableWidgetItem(msg);
    setRowCount(rowCount() + 1);
    setItem(rowCount() - 1, 0, severity);
    setItem(rowCount() - 1, 1, text);
}
