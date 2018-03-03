#include "Theme.hpp"
#include <QPainter>

using namespace p4s;

// QPixmap EclEditTheme::createTypeIcon(const QColor& color)
//{
//    const int size   = 16;
//    const int margin = 2;

//    static const QColor contourColor("#888888");

//    const QRect rect(QPoint(margin, margin),
//                     QPoint(size - margin, size - margin));

//    QPixmap pixmap(size, size);
//    pixmap.fill(QColor(0, 0, 0, 0));
//    QPainter painter(&pixmap);
//    painter.setRenderHint(QPainter::HighQualityAntialiasing);
//    painter.setBrush(contourColor);
//    painter.drawRect(rect);
//    painter.fillRect(rect, color);

//    return pixmap;
//}

QPixmap Theme::createGenericIcon(const QColor& color)
{
    const int size = 16;
    const QColor contourColor = color.darker();

    const QRectF rect(QPointF(-size * 0.2, size * 0.4), QPointF(size * 0.2, -size * 0.4));

    QPixmap pixmap(size, size);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pixmap);
    painter.translate(size / 2.0, size / 2.0);
    painter.rotate(45);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);

    painter.setBrush(contourColor);
    painter.drawRect(rect);
    painter.fillRect(rect, color);

    return pixmap;
}

QPixmap Theme::createRoundGenericIcon(const QColor& color)
{
    const int size = 16;

    const QRectF rect(QPointF(size * 0.15, size * 0.15), QPointF(size * 0.7, size * 0.7));

    QPixmap pixmap(size, size);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pixmap);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                           QPainter::HighQualityAntialiasing);

    QPainterPath path;
    path.addRoundedRect(rect, rect.width() / 2, rect.height() / 2);

    painter.fillPath(path, color);

    return pixmap;
}

Theme Theme::createDefault()
{
    Theme theme;

    theme.lang.dataColor = "#ef8354";
    theme.lang.processingColor = "#678d58";
    theme.lang.exportColor = "#ffcc00";

    theme.lang.keywordColor = "#36e1dd";
    theme.lang.jsKeywordColor = "#f2e033";

    theme.lang.scriptObjectColor = "#35AC19";
    theme.lang.propertyColor = "#35AC19";

    theme.lang.dataIcon = createGenericIcon(theme.lang.dataColor);
    theme.lang.processingIcon = createGenericIcon(theme.lang.processingColor);
    theme.lang.exportIcon = createGenericIcon(theme.lang.exportColor);

    theme.lang.dataIconRound = createRoundGenericIcon(theme.lang.dataColor);
    theme.lang.processingIconRound = createRoundGenericIcon(theme.lang.processingColor);
    theme.lang.exportIconRound = createRoundGenericIcon(theme.lang.exportColor);

    theme.backgroundColor = "#2a292a";

    theme.textEdit.backgroundColor = "#363636";
    theme.textEdit.textColor = "#efeeef";
    theme.textEdit.font = QFont("Monospace");

    theme.textEdit.highlightColor = Qt::yellow;
    theme.textEdit.highlightColor.setAlpha(50);
    theme.textEdit.outlineColor = Qt::magenta;
    theme.textEdit.literalColor = "#22AAFF";
    theme.textEdit.commentColor = "#AAAAAA";

    theme.textEdit.errorColor = "#9e0039";
    theme.textEdit.warningColor = "#ffa500";

    return theme;
}
