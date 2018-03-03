#include "FoldingExtraArea.hpp"
#include "FoldedTextEdit.hpp"
#include "LineNumbersExtraArea.hpp"

#include "../Style.hpp"

#include <QPainter>
#include <QCursor>
#include <QTextBlock>
#include <QPixmap>

using namespace evt;

FoldingExtraArea::FoldingExtraArea(FoldedTextEdit* editor)
    : QWidget(editor), _editor(editor), _lastActiveBlock(-1), _isHighlighing(false)
{
    setMouseTracking(true);
    _currentArea = NULL;

    _backgroundColor = "#222222";
    _foldingMarkersColor = QColor(200, 40, 0, 75);
    _foldingMarkersContourColor = QColor(40, 40, 40);
}
QSize FoldingExtraArea::sizeHint() const
{
    return QSize(_editor->foldingAreaWidth(), 0);
}

void FoldingExtraArea::applyStyle(const Style& s)
{
    _backgroundColor = s.theme.textEdit.backgroundColor;
    _foldingMarkersContourColor = s.theme.primaryColor.darker();
    _foldingMarkersColor = s.theme.primaryColor;
    _foldingMarkersColor.setAlpha(100);

    update();
}

inline void findCollapsedAreas(const FoldingArea& top, QList<FoldingArea>& areas)
{
    foreach (const FoldingArea& area, top.subFoldingAreas) {
        if (!area.isExpanded) {
            areas << area;
        }
        else {
            findCollapsedAreas(area, areas);
        }
    }
}

QList<FoldingArea> FoldingExtraArea::findCollapsedAreas() const
{
    QList<FoldingArea> areas;

    foreach (const FoldingArea& area, _topFoldingAreas) {
        if (!area.isExpanded) {
            areas << area;
        }
        else {
            ::findCollapsedAreas(area, areas);
        }
    }
    return areas;
}

void FoldingExtraArea::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    // painter.save();
    painter.fillRect(contentsRect(), _backgroundColor);
    _drawnMarkers.clear();
    for (int i = 0; i < _topFoldingAreas.size(); ++i)
        drawFoldingAreaMarker(&painter, _topFoldingAreas[i]);
    if (_currentArea) drawFoldingAreaLimiters(&painter, *_currentArea);
}
void FoldingExtraArea::mousePressEvent(QMouseEvent*)
{
    if (!_currentArea) return;
    _currentArea->isExpanded = !_currentArea->isExpanded;
    Q_EMIT foldingRequest(_currentArea->begin, _currentArea->end - 1, _currentArea->isExpanded);
}
void FoldingExtraArea::mouseMoveEvent(QMouseEvent* e)
{
    if (setCurrentFoldingBlock(e->pos())) {
        Q_EMIT highlightBlockRequest(_currentArea->begin - 1, _currentArea->end - 1);
        _isHighlighing = true;
    }
    else {
        if (_isHighlighing) {
            Q_EMIT unHighlightRequest();
            _isHighlighing = false;
        }
    }
    update();
}
void FoldingExtraArea::setCurrentFoldingArea(int block)
{
    if (block == _lastActiveBlock) return;
    _lastActiveBlock = block;
    FoldingArea* old = _currentArea;
    _currentArea = lookAreaContaining(block + 1);
    if (old != _currentArea)  // update is needed even if m_currentArea is null
        update();
}

void FoldingExtraArea::uncollapseArea(int begin)
{
    setCurrentFoldingArea(begin);
    mousePressEvent(nullptr);
}

bool FoldingExtraArea::setCurrentFoldingBlock(const QPoint& pos)
{
    QTextBlock block = _editor->cursorForPosition(QPoint(0, pos.y())).block();
    if (!block.isValid()) return false;
    _currentArea = lookAreaContaining(block.blockNumber() + 1);
    if (_currentArea)
        return true;
    else
        return false;
}
void FoldingExtraArea::leaveEvent(QEvent*)
{
    Q_EMIT unHighlightRequest();
    _isHighlighing = false;
    _currentArea = NULL;
    update();
    int tmpBlockNumber = _lastActiveBlock;
    _lastActiveBlock = -1;  // to foce update
    setCurrentFoldingArea(tmpBlockNumber);
}
void FoldingExtraArea::clearFoldingAreas()
{
    _topFoldingAreas.clear();
}
void FoldingExtraArea::addFoldingArea(int begin, int end, bool collapsed)
{
    FoldingArea area(begin, end);
    area.isExpanded = !collapsed;
    for (int i = 0; i < _topFoldingAreas.size(); ++i)
        if (_topFoldingAreas[i].contains(area)) {
            _topFoldingAreas[i].addSubFoldingArea(area);
            if (collapsed) {
                Q_EMIT foldingRequest(begin, end - 1, false);
            }
            return;
        }
    _topFoldingAreas << area;
    if (collapsed) {
        Q_EMIT foldingRequest(begin, end - 1, false);
    }
}
FoldingArea* FoldingExtraArea::lookAreaContaining(int line)
{
    FoldingArea* area(NULL);
    for (int i = 0; i < _topFoldingAreas.size(); ++i)
        if (_topFoldingAreas[i].contains(line)) {
            area = &_topFoldingAreas[i];
            continue;
        }
    if (area) {
        return area->findLine(line);
    }
    return area;
}
QRect FoldingExtraArea::rectFromLine(int line) const
{
    QTextBlock block = _editor->document()->findBlockByNumber(line - 1);
    if (!block.isValid()) return QRect();
    qreal top = _editor->blockBoundingGeometry(block).translated(_editor->contentOffset()).top();
    qreal bottom =
        _editor->blockBoundingGeometry(block).translated(_editor->contentOffset()).bottom();
    int w(width());
    QPoint topLeft(0, top);
    return QRect(topLeft, QSize(w, bottom - top));
}
void FoldingExtraArea::drawFoldingMarker(QPainter* painter, const QRect& r, bool expanded) const
{
    QPainterPath painterPath;

    QPointF p1, p2, p3;

    QPoint center = r.center();
    int size = qMin(r.width(), r.height()) * 0.5;

    QRect paintRect;

    paintRect.setTopLeft(center - QPoint(size / 2, size / 2));
    paintRect.setBottomRight(center + QPoint(size / 2, size / 2));

    if (expanded) {  // to the bottom
        p1 = paintRect.topLeft();
        p2 = paintRect.topRight();
        p3 = (paintRect.bottomLeft() + paintRect.bottomRight()) / 2;
    }
    else {  // to the left
        p1 = paintRect.topLeft();
        p2 = (paintRect.topRight() + paintRect.bottomRight()) / 2;
        p3 = paintRect.bottomLeft();
    }

    QPolygonF polygon;
    polygon << p1 << p2 << p3;
    painterPath.addPolygon(polygon);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(
        painterPath, QColor(255 - _backgroundColor.red(), 255 - _backgroundColor.green(),
                            255 - _backgroundColor.blue())

    );
}
void FoldingExtraArea::drawFoldingAreaMarker(QPainter* painter, const FoldingArea& area)
{
    QRect r = rectFromLine(area.begin);
    if (r.isNull()) return;
    if (!_drawnMarkers.contains(area.begin)) {
        _drawnMarkers.insert(area.begin);
        drawFoldingMarker(painter, r, area.isExpanded);
    }
    if (!area.isExpanded) return;
    for (int i = 0; i < area.subFoldingAreas.size(); ++i)
        drawFoldingAreaMarker(painter, area.subFoldingAreas[i]);
}
void FoldingExtraArea::drawFoldingAreaLimiters(QPainter* painter, const FoldingArea& area)
{
    QRect top(rectFromLine(area.begin));
    QRect bottom(rectFromLine(area.end));

    if (top.isNull() || bottom.isNull()) return;
    QRect rect;
    if (area.isExpanded) {
        rect.setTopLeft(top.topLeft());
        rect.setBottomRight(bottom.bottomRight());
    }
    else
        rect = top;
    rect.adjust(1, 0, -2, 0);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    int radius = std::max(rect.width() / 10, 2);
    path.addRoundedRect(rect, radius, radius);
    painter->fillPath(path, _foldingMarkersColor);
    QPen pen;
    pen.setColor(_foldingMarkersContourColor);
    pen.setWidthF(.6);
    painter->setPen(pen);
    painter->drawPath(path);
}

bool FoldingArea::contains(int line) const
{
    return begin <= line && end >= line;
}

bool FoldingArea::contains(const FoldingArea& another) const
{
    return begin <= another.begin && end >= another.end;
}

bool FoldingArea::operator==(const FoldingArea& another) const
{
    return begin == another.begin && end == another.end;
}

void FoldingArea::addSubFoldingArea(const FoldingArea& area)
{
    for (int i = 0; i < subFoldingAreas.size(); ++i)
        if (subFoldingAreas[i].contains(area)) {
            subFoldingAreas[i].addSubFoldingArea(area);
            return;
        }
    subFoldingAreas << area;
}

FoldingArea* FoldingArea::findLine(int line)
{
    if (!isExpanded) return this;
    for (int i = 0; i < subFoldingAreas.size(); ++i)
        if (subFoldingAreas[i].contains(line)) return subFoldingAreas[i].findLine(line);
    return this;
}
