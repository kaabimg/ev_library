#pragma once

#include <evt/gui/textedit/Utils.hpp>
#include <ev/core/pimpl.hpp>

#include <QPlainTextEdit>

namespace evt {

class LineNumbersExtraArea;
class FoldingExtraArea;
class File;
class Style;

class FoldedTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    using ExtraSelection = QTextEdit::ExtraSelection;
    using KeySequenceAction = std::function<void()>;

    enum ExtraSelectionType {
        TextFindSelection = 1,
        CurrentLineSelection = 1 << 1,
        BlockHighlightSelection = 1 << 2,
        SyntaxHighlightSelection = 1 << 3,
        ErrorSelection = 1 << 4,
        WarningSelection = 1 << 5,
        ParenthesesMatchingSelection = 1 << 6,

        AllSelections = TextFindSelection | CurrentLineSelection | BlockHighlightSelection |
                        SyntaxHighlightSelection |
                        ErrorSelection |
                        WarningSelection |
                        ParenthesesMatchingSelection
    };
    Q_ENUMS(ExtraSelectionType)

    struct Overlay {
        enum Type { Selection, Search } type;
        QRect rect;
    };

    explicit FoldedTextEdit(File* file, QWidget* parent = 0);
    ~FoldedTextEdit();

    File* file() const;

    void applyStyle(const Style&);

    LineNumbersExtraArea* linesNumbersArea() const;
    FoldingExtraArea* foldingArea() const;
    QList<QAction*> editActions() const;
    int currentLineNumber() const;

    ExtraSelection createExtraSelection(int line) const;
    ExtraSelection createExtraSelection(const TextPosition& position) const;
    ExtraSelection createExtraSelection(int startPosition, int size) const;

    void addOverlay(const Overlay&);
    void clearOverlays();
    bool hasOverlays() const;

    int linesNumbersAreaWidth() const;
    int foldingAreaWidth() const;
    int statusWidgetHeight() const;

    int lineHeight() const;

    void monitorSequence(QKeySequence sequence, const KeySequenceAction& action);

    int lineForPos(uint pos);

public Q_SLOTS:

    void setExtraSelections(const QList<ExtraSelection>& selections, ExtraSelectionType type);

    void addExtraSelections(const QList<ExtraSelection>& selections, ExtraSelectionType type);

    void clearExtraSelections(int type);

    void addFoldingArea(int begin, int end, bool collapsed = false);
    void addFoldingAreaByAbsolutePosition(uint begin, uint end, bool collapsed = false);

    void clearFoldingAreas();
    void updateFoldingArea();
    void gotoLine(int lineNumber);
    void gotoPosition(uint line, uint column);
    void gotoAbsolutePosition(uint position);

    void clearFindTextSelection();
    void updateExtraSelections();

Q_SIGNALS:
    void fontPointSizeChanged(int);
    void cursorPositionChanged(uint, uint);

protected:
    void resizeEvent(QResizeEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* e);

    void paintEvent(QPaintEvent* event);
    void paintFoldingMarkers(const QTextBlock& firstBlock,
                             const QTextBlock& lastBlock,
                             int begin,
                             int end);

    void paintOverlays(QPainter*);

    void mousePressEvent(QMouseEvent* e);

private Q_SLOTS:
    void updateLayout();
    void updateExtraArea(const QRect&, int);
    void highlightCurrentLine();
    virtual void highlightBlock(int, int);
    void onCursorPositionChanged();
    void showOrHideBlocks(int, int, bool);

private:
    QTextEdit::ExtraSelection currentLineSelection() const;
    QTextEdit::ExtraSelection currentExecutionLineSelection() const;
    int baseLineNumber() const;
    void setBaseLineNumber(int base);

private:
    friend class LineNumbersExtraArea;
    friend class FoldingExtraArea;

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
