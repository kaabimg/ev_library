#pragma once

namespace evt
{
// clang-format off
static const char* statusBarButtonStyleSheet =
R"(
QPushButton {
    color: <theme.foregroundColor>;
    border-style: outset;
}

QPushButton:checked, QPushButton:hover {
    background-color: <theme.statusBar.buttonCheckedBackgroundColor>;
    border-style: outset;
}

)";

static const char* toolbarButtonStyleSheet =
R"(
QPushButton {
    color: <theme.foregroundColor>;
    background-color: <theme.background.dark>;
    border-style: outset;
}

QPushButton:checked, QPushButton:hover {
    background-color: <theme.background.light>;
    border-style: outset;
}
QPushButton:pressed {
    background-color: <theme.primaryColor>;
    border-style: outset;
}


)";

// clang-format on
}

