#include "FoldedTextEdit.hpp"

#include "FoldingExtraArea.hpp"
#include "LineNumbersExtraArea.hpp"
#include "TextEditStatusWidget.hpp"

#include "../Style.hpp"
#include "../../core/File.hpp"

#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QTextBlock>
#include <QPainter>
#include <QHash>
#include <QAction>

using namespace evt;

class FoldedTextEditLayout : public QPlainTextDocumentLayout {
public:
    FoldedTextEditLayout(QTextDocument* document) : QPlainTextDocumentLayout(document)
    {
    }
    void emitDocumentSizeChanged()
    {
        Q_EMIT documentSizeChanged(documentSize());
    }
};

struct FoldingAreaMarkerRect : public QRectF {
    FoldingAreaMarkerRect()
    {
    }
    FoldingAreaMarkerRect(qreal left, qreal top, qreal width, qreal height)
        : QRectF(left, top, width, height)
    {
    }
    FoldingArea area;
};

struct FoldedTextEdit::Impl {
    File* file;
    QString errorString;
    LineNumbersExtraArea* linesNumbersArea;
    FoldingExtraArea* foldingArea;
    TextEditStatusWidget* statusWidget;

    int baseLineNumber = 1;
    int linesNumbersAreaToggleBlockNumber;
    int tabWidth = 4;
    QHash<int, int> hiddenBlocks;
    QAction *copyAction, *cutAction, *pasteAction, *undoAction, *redoAction;

    int defaultPointSize;

    QColor highlightColor = Qt::yellow, currentLineHighlightColor;

    QList<QTextEdit::ExtraSelection> currentLineSelection;
    QList<QTextEdit::ExtraSelection> textFindSelection;
    QList<QTextEdit::ExtraSelection> syntaxHighlightSelections;
    QList<QTextEdit::ExtraSelection> blockHighlightSelections;
    QList<QTextEdit::ExtraSelection> warningSelections;
    QList<QTextEdit::ExtraSelection> errorSelections;

    QList<FoldingAreaMarkerRect> foldingMarkersRects;

    QList<FoldedTextEdit::Overlay> overlays;

    QMultiHash<QKeySequence, KeySequenceAction> sequenceActions;

    QList<QTextEdit::ExtraSelection> extraSelections() const
    {
        QList<QTextEdit::ExtraSelection> extraSelections;
        extraSelections << currentLineSelection;
        extraSelections << textFindSelection;
        extraSelections << syntaxHighlightSelections;
        extraSelections << blockHighlightSelections;
        extraSelections << warningSelections;
        extraSelections << errorSelections;
        return extraSelections;
    }
};

FoldedTextEdit::FoldedTextEdit(File* file, QWidget* parent) : QPlainTextEdit(parent)
{
    d->file = file;
    setTabChangesFocus(false);
    setTabStopWidth(QFontMetrics(font()).width(' ') * d->tabWidth);
    document()->setDocumentLayout(new FoldedTextEditLayout(document()));

    d->linesNumbersArea = new LineNumbersExtraArea(this);
    d->foldingArea = new FoldingExtraArea(this);
    d->statusWidget = new TextEditStatusWidget(this);

    connect(this, &FoldedTextEdit::blockCountChanged, this, &FoldedTextEdit::updateLayout);
    connect(this, &FoldedTextEdit::updateRequest, this, &FoldedTextEdit::updateExtraArea);

    connect(d->foldingArea, &FoldingExtraArea::highlightBlockRequest, this,
            &FoldedTextEdit::highlightBlock);
    connect(d->foldingArea, &FoldingExtraArea::unHighlightRequest, this,
            &FoldedTextEdit::clearFindTextSelection);
    connect(d->foldingArea, &FoldingExtraArea::foldingRequest, this,
            &FoldedTextEdit::showOrHideBlocks);

    connect(this, &QPlainTextEdit::cursorPositionChanged, this,
            &FoldedTextEdit::onCursorPositionChanged);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this,
            &FoldedTextEdit::highlightCurrentLine);

    connect(this, &FoldedTextEdit::cursorPositionChanged, d->statusWidget,
            &TextEditStatusWidget::setCursorPosition);

    connect(file, &File::modifiedChanged, d->statusWidget,
            &TextEditStatusWidget::setFileModified);

    connect(d->statusWidget, &TextEditStatusWidget::setCursorPositionRequest, this,
            &FoldedTextEdit::gotoPosition);

    d->copyAction = new QAction("Copy", this);
    connect(d->copyAction, &QAction::triggered, this, &FoldedTextEdit::copy);
    connect(this, &FoldedTextEdit::copyAvailable, d->copyAction, &QAction::setEnabled);
    d->copyAction->setEnabled(false);

    d->cutAction = new QAction("Cut", this);
    connect(d->cutAction, &QAction::triggered, this, &FoldedTextEdit::cut);
    connect(this, &FoldedTextEdit::copyAvailable, d->cutAction, &QAction::setEnabled);
    d->cutAction->setEnabled(false);

    d->pasteAction = new QAction("Paste", this);
    connect(d->pasteAction, &QAction::triggered, this, &FoldedTextEdit::paste);

    d->undoAction = new QAction("Undo", this);
    connect(d->undoAction, &QAction::triggered, this, &FoldedTextEdit::undo);
    connect(this, &FoldedTextEdit::undoAvailable, d->undoAction, &QAction::setEnabled);
    d->undoAction->setEnabled(false);

    d->redoAction = new QAction("Redo", this);
    connect(d->redoAction, &QAction::triggered, this, &FoldedTextEdit::redo);
    connect(this, &FoldedTextEdit::redoAvailable, d->redoAction, &QAction::setEnabled);
    d->redoAction->setEnabled(false);

    connect(this, &FoldedTextEdit::textChanged,
            [this] { d->file->setModified(document()->isModified()); });

    d->defaultPointSize = 11;
    updateLayout();

    applyStyle(appStyle());
}

FoldedTextEdit::~FoldedTextEdit()
{
}

File* FoldedTextEdit::file() const
{
    return d->file;
}

void FoldedTextEdit::applyStyle(const Style& style)
{
    d->linesNumbersArea->applyStyle(style);
    d->foldingArea->applyStyle(style);
    d->statusWidget->setFont(style.theme.textEdit.font);
    d->statusWidget->setBackgroundColor(style.theme.textEdit.backgroundColor);
    d->statusWidget->setTextColor(style.theme.textEdit.textColor);
    d->currentLineHighlightColor = style.theme.textEdit.highlightColor;

    QPalette p = palette();
    p.setColor(QPalette::Base, style.theme.textEdit.backgroundColor);
    p.setColor(QPalette::Text, style.theme.textEdit.textColor);

    setPalette(p);
    setFont(style.theme.textEdit.font);
    setTabStopWidth(QFontMetrics(font()).width(' ') * d->tabWidth);
}

QList<QAction*> FoldedTextEdit::editActions() const
{
    QList<QAction*> acts;
    acts << d->copyAction << d->cutAction << d->pasteAction << d->undoAction
         << d->redoAction;
    return acts;
}

int FoldedTextEdit::baseLineNumber() const
{
    return d->baseLineNumber;
}
void FoldedTextEdit::setBaseLineNumber(int base)
{
    d->baseLineNumber = base;
    d->linesNumbersArea->update();
}
int FoldedTextEdit::currentLineNumber() const
{
    return textCursor().blockNumber() + d->baseLineNumber;
}

FoldedTextEdit::ExtraSelection FoldedTextEdit::createExtraSelection(int line) const
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);

    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);

    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::NextWord);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    ExtraSelection selection;
    selection.cursor = cursor;

    return selection;
}

FoldedTextEdit::ExtraSelection FoldedTextEdit::createExtraSelection(
    const TextPosition& position) const
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);

    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, position.start.line - 1);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, position.start.column - 1);

    cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor,
                        position.end.line - position.start.line);

    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, position.end.column);

    ExtraSelection selection;
    selection.cursor = cursor;

    return selection;
}

FoldedTextEdit::ExtraSelection FoldedTextEdit::createExtraSelection(int startPosition,
                                                                    int size) const
{
    QTextCursor cursor = textCursor();

    cursor.setPosition(startPosition);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, size);

    ExtraSelection selection;
    selection.cursor = cursor;

    return selection;
}

void FoldedTextEdit::addOverlay(const FoldedTextEdit::Overlay& overlay)
{
    d->overlays << overlay;
}

void FoldedTextEdit::clearOverlays()
{
    d->overlays.clear();
}

bool FoldedTextEdit::hasOverlays() const
{
    return d->overlays.size();
}

void FoldedTextEdit::setExtraSelections(const QList<QTextEdit::ExtraSelection>& selections,
                                        FoldedTextEdit::ExtraSelectionType type)
{
    switch (type) {
        case CurrentLineSelection: d->currentLineSelection = selections; break;
        case TextFindSelection: d->textFindSelection = selections; break;
        case SyntaxHighlightSelection: d->syntaxHighlightSelections = selections; break;
        case WarningSelection: d->warningSelections = selections; break;
        case ErrorSelection: d->errorSelections = selections; break;

        default: break;
    }
}

void FoldedTextEdit::addExtraSelections(const QList<FoldedTextEdit::ExtraSelection>& selections,
                                        FoldedTextEdit::ExtraSelectionType type)
{
    switch (type) {
        case CurrentLineSelection: d->currentLineSelection << selections; break;
        case TextFindSelection: d->textFindSelection << selections; break;
        case SyntaxHighlightSelection: d->syntaxHighlightSelections << selections; break;
        case WarningSelection: d->warningSelections << selections; break;
        case ErrorSelection: d->errorSelections << selections; break;

        default: break;
    }
}

void FoldedTextEdit::clearExtraSelections(int type)
{
    if (type & CurrentLineSelection) {
        d->currentLineSelection.clear();
    }
    if (type & TextFindSelection) {
        d->textFindSelection.clear();
    }
    if (type & SyntaxHighlightSelection) {
        d->syntaxHighlightSelections.clear();
    }
    if (type & ErrorSelection) {
        d->errorSelections.clear();
    }
    if (type & WarningSelection) {
        d->warningSelections.clear();
    }
}
void FoldedTextEdit::gotoLine(int lineNumber)
{
    int blockNumber = lineNumber - d->baseLineNumber;
    const QTextBlock& block = document()->findBlockByNumber(blockNumber);
    if (block.isValid()) {
        setTextCursor(QTextCursor(block));
        centerCursor();
    }
}

void FoldedTextEdit::gotoPosition(uint line, uint column)
{
    int blockNumber = line - d->baseLineNumber;
    const QTextBlock& block = document()->findBlockByNumber(blockNumber);
    if (block.isValid()) {
        QTextCursor cursor(block);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column - 1);
        setTextCursor(cursor);
    }
}

void FoldedTextEdit::gotoAbsolutePosition(uint position)
{
    const QTextBlock& block = document()->findBlock(position);
    if (block.isValid()) {
        QTextCursor cursor(block);
        setTextCursor(cursor);
    }
}

void FoldedTextEdit::clearFindTextSelection()
{
    clearExtraSelections(FoldedTextEdit::TextFindSelection);
}

void FoldedTextEdit::updateLayout()
{
    QRect cr = contentsRect();

    QRect statusWidgetRect(cr.topLeft(), cr.topRight() + QPoint(0, statusWidgetHeight()));

    d->statusWidget->setGeometry(statusWidgetRect);

    QRect foldingAreaRect(statusWidgetRect.bottomLeft(),
                          cr.bottomLeft() + QPoint(foldingAreaWidth(), 0));

    d->foldingArea->setGeometry(foldingAreaRect);

    QRect linesNumbersAreaRect(foldingAreaRect.topRight(),
                               foldingAreaRect.bottomRight() + QPoint(linesNumbersAreaWidth(), 0));

    d->linesNumbersArea->setGeometry(linesNumbersAreaRect);

    setViewportMargins(foldingAreaRect.width() + linesNumbersAreaRect.width() - 1,
                       statusWidgetRect.height() - 1, 0, 0);
}

void FoldedTextEdit::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);
    updateLayout();
}

void FoldedTextEdit::keyPressEvent(QKeyEvent* e)
{
    QKeySequence sequence = e->key() | e->modifiers();

    if (d->sequenceActions.contains(sequence)) {
        for (auto& act : d->sequenceActions.values(sequence)) {
            act();
        }
    }
    else {
        if (e->modifiers().testFlag(Qt::ControlModifier) &&
            e->modifiers().testFlag(Qt::ShiftModifier) && e->key() == Qt::Key_0) {
            QFont f = font();
            f.setPointSize(d->defaultPointSize);
            setFont(f);
            Q_EMIT fontPointSizeChanged(d->defaultPointSize);
        }
        else {
            QPlainTextEdit::keyPressEvent(e);
        }
    }
}

void FoldedTextEdit::updateExtraArea(const QRect& rect, int dy)
{
    if (dy)
        d->linesNumbersArea->scroll(0, dy);
    else
        d->linesNumbersArea->update(0, rect.y(), d->linesNumbersArea->width(),
                                        rect.height());
    if (dy)
        d->foldingArea->scroll(0, dy);
    else
        d->foldingArea->update(0, rect.y(), d->foldingArea->width(), rect.height());
    if (rect.contains(viewport()->rect())) updateLayout();
}
void FoldedTextEdit::highlightCurrentLine()
{
    clearExtraSelections(FoldedTextEdit::CurrentLineSelection);
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(d->currentLineHighlightColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    setExtraSelections(extraSelections, CurrentLineSelection);

    updateExtraSelections();
}

int FoldedTextEdit::linesNumbersAreaWidth() const
{
    int space = 0;
    const QFontMetrics fm(fontMetrics());
    int digits = 1;
    int max = qMax(1, blockCount() + d->baseLineNumber);
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    space += fm.width(QLatin1Char('9')) * digits;
    space += 10;
    return space;
}
int FoldedTextEdit::foldingAreaWidth() const
{
    return 20;
}

int FoldedTextEdit::statusWidgetHeight() const
{
    return 30;
}

int FoldedTextEdit::lineHeight() const
{
    return blockBoundingGeometry(textCursor().block()).height();
}

void FoldedTextEdit::monitorSequence(QKeySequence sequence, const KeySequenceAction& action)
{
    d->sequenceActions.insert(sequence, action);
}

int FoldedTextEdit::lineForPos(uint pos)
{
    QTextBlock block = document()->findBlock(pos);
    if (block.isValid()) return block.blockNumber() + 1;
    return 0;
}

void FoldedTextEdit::updateExtraSelections()
{
    QPlainTextEdit::setExtraSelections(d->extraSelections());
}

/***************************************************************************************/

void FoldedTextEdit::highlightBlock(int beginBlock, int endBlock)
{
    clearExtraSelections(BlockHighlightSelection);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor color = Qt::darkGray;
    color.setAlpha(50);
    selection.format.setBackground(d->highlightColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    QTextCursor originalCursor(textCursor());
    QTextCursor navigationCursor(originalCursor);

    if (navigationCursor.blockNumber() > beginBlock) {
        navigationCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor,
                                      navigationCursor.blockNumber() - beginBlock);
    }
    else {
        navigationCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,
                                      beginBlock - navigationCursor.blockNumber());
    }

    for (int i = beginBlock; i <= endBlock; ++i) {
        selection.cursor = navigationCursor;
        selection.cursor.clearSelection();
        extraSelections.append(selection);
        navigationCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 1);
    }

    setExtraSelections(extraSelections, BlockHighlightSelection);
}

void FoldedTextEdit::addFoldingArea(int begin, int end, bool collapsed)
{
    d->foldingArea->addFoldingArea(begin, end, collapsed);
}

void FoldedTextEdit::addFoldingAreaByAbsolutePosition(uint begin, uint end, bool collapsed)
{
    int beginLine = lineForPos(begin);
    int endLine = lineForPos(end);
    if (beginLine != endLine) addFoldingArea(beginLine, endLine, collapsed);
}

void FoldedTextEdit::clearFoldingAreas()
{
    d->foldingArea->clearFoldingAreas();
    d->hiddenBlocks.clear();
    QTextBlock block = document()->firstBlock();

    bool needUpdate = false;

    while (block.isValid()) {
        if (!block.isVisible()) {
            block.setVisible(true);
            needUpdate = true;
        }
        block = block.next();
    }

    if (needUpdate) {
        update();
    }
}

void FoldedTextEdit::updateFoldingArea()
{
    d->foldingArea->update();
}

void FoldedTextEdit::showOrHideBlocks(int begin, int end, bool visible)
{
    QTextBlock block = document()->findBlockByNumber(begin);
    int position = block.position();
    int length(0);
    if (visible) {
        for (int i = 0; i <= end - begin; ++i) {
            d->hiddenBlocks[block.blockNumber()]--;
            if (!d->hiddenBlocks[block.blockNumber()]) block.setVisible(true);
            length += block.length();
            block = block.next();
        }
    }
    else {
        for (int i = 0; i <= end - begin; ++i) {
            if (!this->d->hiddenBlocks.contains(block.blockNumber()))
                d->hiddenBlocks.insert(block.blockNumber(), 1);
            else
                d->hiddenBlocks[block.blockNumber()]++;
            block.setVisible(false);
            length += block.length();
            block = block.next();
        }
    }
    document()->markContentsDirty(position, length);
}
void FoldedTextEdit::onCursorPositionChanged()
{
    int line = textCursor().block().blockNumber();
    int column = textCursor().positionInBlock();
    d->foldingArea->setCurrentFoldingArea(line);
    d->linesNumbersArea->setCurrentLine(line + 1);

    Q_EMIT cursorPositionChanged(line + 1, column + 1);
}
void FoldedTextEdit::wheelEvent(QWheelEvent* e)
{
    if (e->modifiers().testFlag(Qt::ControlModifier)) {
        QFont font = this->font();
        if (e->delta() > 0) {
            font.setPointSize(font.pointSize() + 1);
            setFont(font);
            Q_EMIT fontPointSizeChanged(font.pointSize());
        }
        else if (e->delta() < 0) {
            font.setPointSize(qMax(4, font.pointSize() - 1));
            setFont(font);
            Q_EMIT fontPointSizeChanged(font.pointSize());
        }
    }
    else {
        QPlainTextEdit::wheelEvent(e);
    }
}

void FoldedTextEdit::paintEvent(QPaintEvent* event)
{
    QPlainTextEdit::paintEvent(event);

    d->foldingMarkersRects.clear();

    QList<FoldingArea> collapsedAreas = d->foldingArea->findCollapsedAreas();

    for (const FoldingArea& area : collapsedAreas) {
        QTextBlock first = document()->findBlockByNumber(area.begin - 1);
        QTextBlock last = document()->findBlockByNumber(area.end - 1);

        if (first.isValid() && last.isValid()) {
            paintFoldingMarkers(first, last, area.begin, area.end);
        }
    }
}

void FoldedTextEdit::paintFoldingMarkers(const QTextBlock& firstBlock,
                                         const QTextBlock& lastBlock,
                                         int begin,
                                         int end)
{
    qreal top = blockBoundingGeometry(firstBlock).translated(contentOffset()).top();
    QTextLayout* layout = firstBlock.layout();
    QTextLine line = layout->lineAt(layout->lineCount() - 1);
    QRectF lineRect = line.naturalTextRect().translated(0, top);
    lineRect.adjust(0, 0, -1, -1);
    QString firtHiddenLineText = firstBlock.next().text().trimmed();
    bool showBrace = firtHiddenLineText.startsWith('{');
    QString rectText =
        showBrace ? "{..." + lastBlock.text().trimmed() : "..." + lastBlock.text().trimmed();

    FoldingAreaMarkerRect collapseRect(lineRect.right() + 12, lineRect.top(),
                                       fontMetrics().width(rectText), lineRect.height());

    collapseRect.area.begin = begin;
    collapseRect.area.end = end;

    d->foldingMarkersRects << collapseRect;
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(.5, .5);

    painter.drawRoundedRect(collapseRect, 2.5, 2.5);
    painter.translate(-.5, -.5);
    painter.drawText(collapseRect.adjusted(1, 1, -1, -1), Qt::AlignCenter, rectText);
}

void FoldedTextEdit::paintOverlays(QPainter* painter)
{
    QColor color = d->highlightColor;
    color.setAlpha(70);
    foreach (const Overlay& overlay, d->overlays) {
        if (rect().contains(overlay.rect)) {
            QPainterPath path;
            path.addRoundedRect(overlay.rect, 2, 2);
            painter->fillPath(path, color);
            painter->drawPath(path);
        }
    }
}

void FoldedTextEdit::mousePressEvent(QMouseEvent* e)
{
    foreach (const FoldingAreaMarkerRect& rect, d->foldingMarkersRects) {
        if (rect.contains(e->pos())) {
            d->foldingArea->uncollapseArea(rect.area.begin);
            break;
        }
    }
    QPlainTextEdit::mousePressEvent(e);
}

LineNumbersExtraArea* FoldedTextEdit::linesNumbersArea() const
{
    return d->linesNumbersArea;
}

FoldingExtraArea* FoldedTextEdit::foldingArea() const
{
    return d->foldingArea;
}
