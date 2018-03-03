#include "LineNumbersExtraArea.hpp"
#include "FoldedTextEdit.hpp"
#include "../Style.hpp"

#include <QPainter>
#include <QCursor>
#include <QTextBlock>

using namespace evt;

LineNumbersExtraArea::LineNumbersExtraArea(FoldedTextEdit* edit)
    : QWidget(edit), _editor(edit), _currentLine(-1)
{
    setMouseTracking(true);
}
QSize LineNumbersExtraArea::sizeHint() const
{
    return QSize(_editor->linesNumbersAreaWidth(), 0);
}

void LineNumbersExtraArea::applyStyle(const Style& s)
{
    _backgroundColor = s.theme.textEdit.backgroundColor;
    _numbersColor = s.theme.textEdit.textColor;
}

void LineNumbersExtraArea::setCurrentLine(int line)
{
    if (_currentLine != line) {
        _currentLine = line;
        update();
    }
}

void LineNumbersExtraArea::paintEvent(QPaintEvent* e)
{
    QRect rect = e->rect();
    QPalette pal = palette();
    pal.setCurrentColorGroup(QPalette::Active);
    QPainter painter(this);

    QFont normalFont = _editor->font();
    QFont currentLineFont = normalFont;
    currentLineFont.setBold(true);
    painter.fillRect(contentsRect(), _backgroundColor);
    const QFontMetrics fm(_editor->fontMetrics());
    int linesNumbersAreaWidth = width();
    painter.setRenderHint(QPainter::Antialiasing);
    QTextBlock block = _editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    qreal top = _editor->blockBoundingGeometry(block).translated(_editor->contentOffset()).top();
    qreal bottom = top + _editor->blockBoundingRect(block).height();

    QColor notCurrentLineColor = _numbersColor;
    notCurrentLineColor.setAlpha(150);

    painter.setPen(notCurrentLineColor);
    painter.setFont(normalFont);

    bool changed = false;

    while (block.isValid() && top <= rect.bottom()) {
        if (block.isVisible() && bottom >= rect.top()) {
            int lineNumber = blockNumber + _editor->baseLineNumber();
            QString number = QString::number(lineNumber);
            if (lineNumber == _currentLine) {
                painter.setPen(_numbersColor);
                painter.setFont(currentLineFont);
                changed = true;
            }
            else if (changed) {
                painter.setPen(notCurrentLineColor);
                painter.setFont(normalFont);
            }

            painter.drawText(rect.x(), (int)top, rect.x() + linesNumbersAreaWidth - 4, fm.height(),
                             Qt::AlignCenter, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + _editor->blockBoundingRect(block).height();
        ++blockNumber;
    }
}
void LineNumbersExtraArea::mousePressEvent(QMouseEvent* e)
{
    QTextCursor cursor = _editor->cursorForPosition(QPoint(0, e->pos().y()));
    cursor.setPosition(cursor.block().position());
}
void LineNumbersExtraArea::mouseMoveEvent(QMouseEvent* e)
{
    QTextCursor cursor = _editor->cursorForPosition(QPoint(0, e->pos().y()));
    cursor.setPosition(cursor.block().position());
}
void LineNumbersExtraArea::mouseReleaseEvent(QMouseEvent* e)
{
    QTextCursor cursor = _editor->cursorForPosition(QPoint(0, e->pos().y()));
    cursor.setPosition(cursor.block().position());
}
bool LineNumbersExtraArea::event(QEvent* e)
{
    if (e->type() == QEvent::ToolTip) {
        // ### show informations
        return true;
    }
    return QWidget::event(e);
}
