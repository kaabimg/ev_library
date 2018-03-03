#pragma once

namespace evt {

static const char* lineEditStyleSheet =
R"(
QLineEdit {
    background-color: <theme.background.light>;
    color : <theme.foregroundColor>;
    border: 1px solid <theme.background.normal>;
    border-radius: 0px;
}

)";
}
