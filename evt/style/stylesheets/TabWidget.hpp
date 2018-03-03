#pragma once

namespace evt
{
// clang-format off
static const char* tabWidgetStyleSheet =
R"(

QTabWidget::pane {
    border: 0px ;
    margin : -1 -1 -1 -1;
    background: <theme.background.light>;
}

QTabBar::tab {
    background: <theme.background.light>;
    color: <theme.foregroundColor>;
    width: 150px
}

QTabBar::tab:selected {
    background: <theme.background.normal>;
    color: <theme.foregroundColor>;
    font : bold;
}

)";
// clang-format on
}

