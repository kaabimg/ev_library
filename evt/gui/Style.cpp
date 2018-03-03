#include "Style.hpp"

using namespace evt;

#define STRINGIFY(var) #var

#define SET_COLOR(str, var) str.replace("<" STRINGIFY(var) ">", var.name(QColor::HexArgb));

#define SET_SIZE(str, var) str.replace("<" STRINGIFY(var) ">", QString::number(var));

QString Style::adaptStyleSheet(const char* code) const
{
    QString str = code;

    /// Colors
    SET_COLOR(str, theme.background.dark);
    SET_COLOR(str, theme.background.light);
    SET_COLOR(str, theme.background.normal);
    SET_COLOR(str, theme.foregroundColor);
    SET_COLOR(str, theme.primaryColor);
    SET_COLOR(str, theme.secondaryColor);
    SET_COLOR(str, theme.scrollBarHandleColor);
    SET_COLOR(str, theme.scrollBarBackgroundColor);
    SET_COLOR(str, theme.seperatorColor);
    SET_COLOR(str, theme.statusBar.buttonCheckedBackgroundColor);

    /// Sizes
    SET_SIZE(str, sizes.scrollBarSize);

    return str;
}

ChangeNotifier* Style::changeNotifier = nullptr;
Style Style::instance;

Style::Style()
{
    setDefault();
}

QIcon Style::icon(IconType iconType) const
{
    return QIcon(theme.iconPath.at(iconType));
}

void Style::setDefault()
{
    theme.font = QFont("Monospace");
    theme.background.dark = "#2a292a";
    theme.background.normal = "#363636";
    theme.background.light = "#414147";

    theme.foregroundColor = "#efeeef";
    theme.primaryColor = "#3b73cb";
    theme.secondaryColor = "#FF0000";  // TODO

    theme.scrollBarBackgroundColor = theme.background.dark.darker(100);
    theme.scrollBarHandleColor = theme.background.light;

    theme.seperatorColor = theme.background.normal.darker(120);

    theme.statusBar.buttonCheckedBackgroundColor = theme.background.dark;
    theme.statusBar.buttonCheckedBackgroundColor.setAlpha(50);

    theme.textEdit.font = theme.font;
    theme.textEdit.backgroundColor = theme.background.normal;
    theme.textEdit.textColor = theme.foregroundColor;
    theme.textEdit.highlightColor = theme.background.light;
    theme.textEdit.highlightColor.setAlpha(100);
    theme.textEdit.outlineColor = Qt::yellow;
    theme.textEdit.propertyColor = "#CCCCFF";
    theme.textEdit.objectNameColor = "#AAAAFF";

    theme.textEdit.builtInTypeColor = theme.primaryColor;
    theme.textEdit.keyWordColor = "#7efa1b";
    theme.textEdit.numberLiteralColor = "#c04680";
    theme.textEdit.commentColor = "#AAAAAA";

    theme.iconPath[IconType::Home] = ":/style/icons/home.png";
    theme.iconPath[IconType::Edit] = ":/style/icons/edit.png";
    theme.iconPath[IconType::TreeArrowDown] = ":/style/icons/arrow_down.png";
    theme.iconPath[IconType::TreeArrowRight] = ":/style/icons/arrow_right.png";
    theme.iconPath[IconType::Message] = ":/style/icons/message.png";
    theme.iconPath[IconType::SideBar] = ":/style/icons/sidebar.png";
    theme.iconPath[IconType::ConsoleArrow] = ":/style/icons/console_arrow.png";
    theme.iconPath[IconType::ConsoleOutputArrow] = ":/style/icons/console_output_arrow.png";
    theme.iconPath[IconType::Error] = ":/style/icons/error.png";
    theme.iconPath[IconType::Warning] = ":/style/icons/warning.png";

    sizes.mainBarWidth = 60;
    sizes.scrollBarSize = 8;
    sizes.tabBarHeight = 400;
}
