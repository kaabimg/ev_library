#pragma once

#include <QWidget>

namespace evt {

class FoldedTextEdit;
class Style;
class LineNumbersExtraArea : public QWidget {
public:
    LineNumbersExtraArea(FoldedTextEdit* edit);

    QSize sizeHint() const;

    void applyStyle(const Style&);

    void setCurrentLine(int);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    bool event(QEvent* event);

private:
    FoldedTextEdit* _editor;
    QColor _backgroundColor, _numbersColor;
    int _currentLine;
};
}
