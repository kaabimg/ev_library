#pragma once
#include <QColor>
#include <QFont>
#include <QIcon>

namespace p4s {

struct Theme {
    struct Lang {
        QColor dataColor;
        QColor processingColor;
        QColor exportColor;

        QColor scriptObjectColor;
        QColor propertyColor;

        QColor keywordColor;
        QColor jsKeywordColor;

        QIcon dataIcon;
        QIcon processingIcon;
        QIcon exportIcon;

        QIcon dataIconRound;
        QIcon processingIconRound;
        QIcon exportIconRound;
    };

    struct TextEdit {
        QFont font;
        QColor textColor;
        QColor backgroundColor;

        QColor highlightColor;
        QColor outlineColor;
        QColor literalColor;
        QColor commentColor;

        QColor errorColor;
        QColor warningColor;
    };

    Lang lang;
    TextEdit textEdit;
    QColor backgroundColor;

public:
    static Theme createDefault();
    static QPixmap createGenericIcon(const QColor&);
    static QPixmap createRoundGenericIcon(const QColor&);

};

static Theme theme;
}
