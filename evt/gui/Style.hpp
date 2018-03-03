#pragma once

#include <evt/core/ChangeNotifier.hpp>

#include <unordered_map>
#include <any>

#include <QColor>
#include <QIcon>
#include <QFont>

namespace evt {

enum class IconType {
    Home,
    Edit,
    TreeArrowRight,
    TreeArrowDown,
    Message,
    SideBar,
    ConsoleArrow,
    ConsoleOutputArrow,
    Error,
    Warning
};

struct Style {
    static ChangeNotifier* changeNotifier;
    static Style instance;

    struct Theme {
        struct Background {
            QColor normal;
            QColor dark;
            QColor light;
        } background;

        struct TextEdit {
            QColor backgroundColor;
            QColor textColor;
            QColor highlightColor;
            QColor outlineColor;
            QColor numberLiteralColor;
            QColor commentColor;

            QColor propertyColor;
            QColor objectNameColor;

            QColor builtInTypeColor;
            QColor keyWordColor;

            QFont font;
        };

        struct StatusBar {
            QColor buttonCheckedBackgroundColor;
        };

        QColor foregroundColor;
        QColor primaryColor, secondaryColor;
        QColor seperatorColor;
        QColor scrollBarHandleColor, scrollBarBackgroundColor;

        QFont font;

        StatusBar statusBar;
        TextEdit textEdit;
        std::unordered_map<IconType, QString> iconPath;
    };

    struct Sizes {
        int mainBarWidth;
        int tabBarHeight;
        int toolBarHeight;
        int treeItemHeight;
        int scrollBarSize;
    };

public:
    Theme theme;
    Sizes sizes;

private:
    std::any _customData;

public:
    Style();
    void setDefault();
    QIcon icon(IconType) const;
    QString adaptStyleSheet(const char* code) const;

    template <typename T>
    T& customData()
    {
        return std::any_cast<T&>(_customData);
    }

    template <typename T>
    const T& customData() const
    {
        return std::any_cast<const T&>(_customData);
    }

    template <typename T>
    void initCustomData()
    {
        _customData = std::make_any<T>();
    }

    template <typename T>
    void initCustomData(T&& val)
    {
        _customData = std::make_any<T>(std::forward<T>(val));
    }
};

inline void monitorStyleChangeFor(auto objPtr)
{
    Style::changeNotifier->onChange(objPtr, [objPtr]() { objPtr->applyStyle(Style::instance); });
}
inline const Style& appStyle()
{
    return Style::instance;
}
}
