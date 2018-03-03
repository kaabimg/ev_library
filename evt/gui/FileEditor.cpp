#include "FileEditor.hpp"

#include "textedit/FoldedTextEdit.hpp"
#include "textedit/TextFindWidget.hpp"
#include "textedit/FileChangedWidget.hpp"

#include "Style.hpp"
#include "../core/File.hpp"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QTextBlock>

#include <QTimer>

using namespace evt;

struct FileEditor::Impl {
    File* file;
    QVBoxLayout* layout;
    TextFindWidget* findWidget;
    FileChangedWidget* fileChangedWidget;
    FoldedTextEdit* editor;
    QTimer selectionTimer;
    QTextCharFormat mismatchFormat;

    Style::Theme::TextEdit theme;
};

FileEditor::FileEditor(File* file, QWidget* parent) : QWidget(parent)
{
    d->file = file;
    d->layout = new QVBoxLayout;
    d->layout->setMargin(0);
    d->layout->setSpacing(0);
    d->layout->setContentsMargins(0, 0, 0, 0);
    setLayout(d->layout);

    d->fileChangedWidget = new FileChangedWidget(this);
    d->editor = createEditor(file);
    d->findWidget = new TextFindWidget(this);

    d->layout->addWidget(d->fileChangedWidget);
    d->layout->addWidget(d->editor);
    d->layout->addWidget(d->findWidget);

    d->fileChangedWidget->setFixedHeight(50);

    d->fileChangedWidget->hide();
    d->findWidget->hide();

    d->selectionTimer.setInterval(800);
    d->selectionTimer.setSingleShot(true);

    connect(file, &File::changedOutside, d->fileChangedWidget,
            &FileChangedWidget::askForFileChangedOutside);

    connect(file, &File::deletedOutside, d->fileChangedWidget,
            &FileChangedWidget::askForFileDeletedOutside);

    connect(d->editor->document(), &QTextDocument::modificationChanged, file,
            &File::setModified);

    connect(d->fileChangedWidget, &FileChangedWidget::reloadRequest, this, &FileEditor::read);
    connect(d->fileChangedWidget, &FileChangedWidget::closeRequest, this,
            &FileEditor::removeFileRequest);
    connect(d->fileChangedWidget, &FileChangedWidget::saveRequest, this,
            &FileEditor::forceSave);

    connect(d->findWidget, &TextFindWidget::findTextChanged, this,
            [this](const QString& str) { highlightText(str); });

    connect(d->findWidget, &TextFindWidget::finished, textEditor(),
            &FoldedTextEdit::clearFindTextSelection);
    connect(d->findWidget, &TextFindWidget::findNextRequest, this, &FileEditor::findNext);
    connect(d->findWidget, &TextFindWidget::findPreviousRequest, this,
            &FileEditor::findPrevious);

    d->editor->document()->setModified(file->modified());

    d->editor->monitorSequence(QKeySequence::Save, [this] { save(); });
    d->editor->monitorSequence(QKeySequence::Find, [this] { onFindRequest(); });
    d->editor->monitorSequence(QKeySequence::Cancel, [this] { d->findWidget->hide(); });

    connect(d->editor, &FoldedTextEdit::selectionChanged, &d->selectionTimer,
            qOverload<>(&QTimer::start));
    connect(&d->selectionTimer, &QTimer::timeout, this, &FileEditor::onSelectionChanged);

    addActions(d->editor->editActions());

    read();

    monitorStyleChangeFor(this);
    applyStyle(appStyle());
}

FileEditor::~FileEditor()
{
}

void FileEditor::applyStyle(const Style& style)
{
    d->mismatchFormat.setBackground(Qt::magenta);
    d->theme = style.theme.textEdit;
    d->editor->applyStyle(style);
    d->findWidget->applyStyle(style);
    d->fileChangedWidget->applyStyle(style);

    update();
}

FoldedTextEdit* FileEditor::textEditor() const
{
    return d->editor;
}

void FileEditor::save(bool force)
{
    if (force || textEditor()->document()->isModified()) {
        if (d->file->isValid()) {
            if (!d->file->save(textEditor()->toPlainText())) {
                //                System system("File editor");
                //                system.error(System::Message() << "Failed to save file" <<
                //                _impl->file->path() << ":\n"
                //                                               << _impl->file->errorString());
            }
            else {
                textEditor()->document()->setModified(false);

                d->file->unwatch();
                d->file->watch();
            }
        }
        else {
            saveAs();
        }
    }
}

void FileEditor::forceSave()
{
    save(true);
}

void FileEditor::saveAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save as ...");

    if (!filePath.isEmpty()) {
        if (File::save(textEditor()->toPlainText(), filePath)) {
            d->file->setPath(filePath);
            d->file->setModified(false);
        }
        else {
            //            System system("File editor");
            //            system.error(System::Message() << "Failed to save file" << filePath);
        }
    }
}

void FileEditor::read()
{
    bool ok = false;

    if (!d->file->exists()) {
        return;
    }

    QString data = d->file->readContent(&ok);
    if (ok) {
        textEditor()->setPlainText(data);
    }
    else {
        //        System system("File editor");
        //        system.error(System::Message() << "Failed to read file" << _impl->file->path() <<
        //        ":\n"
        //                                       << _impl->file->errorString());
    }

    d->file->rewatch();
}

void FileEditor::highlightText(const QString& highlighText, bool outlineOnly)
{
    FoldedTextEdit* e = textEditor();

    if (highlighText.isEmpty()) {
        e->clearExtraSelections(FoldedTextEdit::TextFindSelection);
        e->updateExtraSelections();
        return;
    }

    QTextCursor initialCursor = e->textCursor();
    QList<QTextEdit::ExtraSelection> extraSelections;
    for (int i = 0; i < e->document()->blockCount(); i++) {
        QTextBlock block = e->document()->findBlockByNumber(i);
        if (block.isValid()) {
            QString text = block.text();
            int p;
            int start = 0;

            while ((p = text.indexOf(highlighText, start)) != -1) {
                start = p + highlighText.size();
                int pos = block.position() + p;
                QTextEdit::ExtraSelection selection;
                selection.cursor = QTextCursor(e->document());
                selection.cursor.setPosition(pos);
                selection.cursor.setPosition(pos + highlighText.length(), QTextCursor::KeepAnchor);

                if (outlineOnly) {
                    selection.format.setBackground(d->theme.outlineColor);
                }
                else {
                    selection.format.setBackground(d->theme.highlightColor);
                }

                extraSelections.append(selection);
            }
        }
    }

    disconnect(textEditor(), &FoldedTextEdit::selectionChanged, this,
               &FileEditor::onSelectionChanged);
    e->setExtraSelections(extraSelections, FoldedTextEdit::TextFindSelection);
    e->setTextCursor(initialCursor);
    e->updateExtraSelections();
    connect(textEditor(), &FoldedTextEdit::selectionChanged, this, &FileEditor::onSelectionChanged);
}

void FileEditor::findNext(const QString& text)
{
    textEditor()->find(text);
}

void FileEditor::findPrevious(const QString& text)
{
    textEditor()->find(text, QTextDocument::FindBackward);
}

void FileEditor::onFindRequest()
{
    d->findWidget->fillAndShow(textEditor()->textCursor().selectedText());
}

void FileEditor::onSelectionChanged()
{
    if (d->editor->hasFocus()) {
        QString text = d->editor->textCursor().selectedText();
        highlightText(text, true);
    }
}

void FileEditor::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
}

FoldedTextEdit* FileEditor::createEditor(File* file)
{
    return new FoldedTextEdit(file, this);
}
