#include "style.hpp"
#include "main_window.hpp"

#include <qtoolbar.h>


void ev::atk::apply_style(qtoolbar* toobar)
{
    toobar->setStyleSheet(QString("QToolBar { background-color: %0;} "
                                  "QToolButton{color: rgba(255,255,255,200);} "
                                  "QToolButton:hover{background: rgba(255,255,255,40);}")
                              .arg(atk_main_window->window_palette().dark_gray.name()));

    toobar->setFixedHeight(atk_main_window->window_sizes().toolbar_height);

}


