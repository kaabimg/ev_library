#include "style.hpp"
#include "main_window.hpp"

#include <qtoolbar.h>
#include <qdebug.h>

ev::atk::qstring color_to_string(const ev::atk::qcolor& c)
{
    return QString("rgba(%0,%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
}

void ev::atk::apply_style(qtoolbar* toobar)
{
    toobar->setStyleSheet(
        QString("QToolBar { background-color: %0; } "
                "QToolButton { color: %1; } "
                "QToolButton:hover { background-color: %2 ; color: %1 }")
            .arg(color_to_string(atk_window_palette.background_color()),
                 color_to_string(atk_window_palette.text_color()),
                 color_to_string(atk_window_palette.highlight_color())));

    toobar->setFixedHeight(atk_main_window->window_sizes().toolbar_height );
}
