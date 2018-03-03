#pragma once

#include <QWidget>
#include <QList>
#include <QSet>

namespace evt {

class FoldedTextEdit;
class Style;

struct FoldingArea {
    FoldingArea(int b, int e) : begin(b), end(e), isExpanded(true)
    {
    }
    FoldingArea() : begin(-1), end(-1), isExpanded(true)
    {
    }
    bool isValid()
    {
        return begin > -1 && end > -1;
    }
    bool contains(int line) const;
    bool contains(const FoldingArea& another) const;
    bool operator==(const FoldingArea& another) const;
    void addSubFoldingArea(const FoldingArea& area);
    FoldingArea* findLine(int line);
    int begin, end;
    bool isExpanded;
    QList<FoldingArea> subFoldingAreas;
};

class FoldingExtraArea : public QWidget {
    Q_OBJECT

public:
    FoldingExtraArea(FoldedTextEdit* edit);
    QSize sizeHint() const;
    void applyStyle(const Style&);

    QList<FoldingArea> findCollapsedAreas() const;

public Q_SLOTS:
    void clearFoldingAreas();
    void addFoldingArea(int begin, int end, bool collapsed);
    void setCurrentFoldingArea(int block);
    void uncollapseArea(int beginLine);

Q_SIGNALS:
    void highlightBlockRequest(int, int);
    void unHighlightRequest();
    void foldingRequest(int begin, int end, bool visible);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);
    void drawFoldingMarker(QPainter*, const QRect&, bool) const;
    void drawFoldingAreaMarker(QPainter*, const FoldingArea&);
    void drawFoldingAreaLimiters(QPainter*, const FoldingArea&);
    QRect rectFromLine(int) const;

private:
    bool setCurrentFoldingBlock(const QPoint&);
    FoldingArea* lookAreaContaining(int line);
    FoldedTextEdit* _editor;
    FoldingArea* _currentArea;
    int _lastActiveBlock;
    QList<FoldingArea> _topFoldingAreas;
    QSet<int> _drawnMarkers;
    bool _isHighlighing;
    QColor _backgroundColor, _foldingMarkersColor, _foldingMarkersContourColor;
};
}
