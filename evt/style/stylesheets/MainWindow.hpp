#pragma once

namespace evt
{
// clang-format off
static const char* toolBarStyleSheet =
R"(
QToolBar { background-color: <theme.background.light>;}
QToolButton:hover {background-color: <theme.background.dark>;}
QToolButton:selected {background-color: <theme.background.dark>;}
)";



static const char* statusBarStyleSheet =
R"(
QStatusBar { background-color: <theme.primaryColor>;}
)";

static const char* scrollBarStyleSheet =
R"(
QScrollBar::vertical {
     border: none;
     background: <theme.scrollBarBackgroundColor>;
     width: <sizes.scrollBarSize>px;
     margin: 0px;
}

QScrollBar::horizontal {
     border: none;
     background: <theme.scrollBarBackgroundColor>;
     height: <sizes.scrollBarSize>px;
     margin: 0px;
}

QScrollBar::handle {
     background: <theme.scrollBarHandleColor>;
     min-width: <sizes.scrollBarSize>px;
     min-height: <sizes.scrollBarSize>px;
}

QScrollBar::add-line,  QScrollBar::sub-line {
     background:  <theme.scrollBarBackgroundColor>;
     border: none;
}

QScrollBar::add-page, QScrollBar::sub-page {
     background: <theme.scrollBarBackgroundColor>;
}
)";

static const char* mainwindowStyleSheet =
R"(
QScrollBar::vertical {
     border: none;
     background: <theme.scrollBarBackgroundColor>;
     width: <sizes.scrollBarSize>px;
     margin: 0px;
}

QScrollBar::horizontal {
     border: none;
     background: <theme.scrollBarBackgroundColor>;
     height: <sizes.scrollBarSize>px;
     margin: 0px;
}

QScrollBar::handle {
     background: <theme.scrollBarHandleColor>;
     min-width: <sizes.scrollBarSize>px;
     min-height: <sizes.scrollBarSize>px;
}

QScrollBar::add-line,  QScrollBar::sub-line {
     background:  <theme.scrollBarBackgroundColor>;
     border: none;
}

QScrollBar::add-page, QScrollBar::sub-page {
     background: <theme.scrollBarBackgroundColor>;
}
)";

static const char* toolboxStyleSheet = R"(
QToolBox {
     background-color: <theme.background.normal>;
}

QToolBox::tab {
    padding-left: 0px;
    background-color: <theme.background.normal>;
}

QToolBox::tab::title {
      color: <theme.foregroundColor>;
};
)";

// clang-format on
}

