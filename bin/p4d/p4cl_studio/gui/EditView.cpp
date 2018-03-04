#include "EditView.hpp"
#include <evt/gui/Style.hpp>
#include <evt/gui/SplitWidget.hpp>
#include <evt/gui/TabWidget.hpp>

#include <evt/gui/FileSystemTree.hpp>
#include <evt/gui/FileEditor.hpp>
#include <evt/gui/textedit/FoldedTextEdit.hpp>
#include <evt/core/File.hpp>
#include <evt/style/stylesheets/MainWindow.hpp>
#include <evt/style/stylesheets/TreeView.hpp>
#include <evt/style/stylesheets/Button.hpp>
#include <evt/gui/Widget.hpp>

#include <p4cl/compiler/Compiler.hpp>

#include "editor/SyntaxHighlighter.hpp"
#include "editor/DynamicSyntaxHighlighter.hpp"
#include "editor/CodeAssistant.hpp"
#include "editor/Completer.hpp"
#include "editor/ProgramTreeModel.hpp"
#include "editor/CompilationLogModel.hpp"
#include "editor/CompletionModel.hpp"

#include "graphview/GraphView.hpp"

#include "UiUtils.hpp"

#include <QTreeView>
#include <QListView>

#include <QToolBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <QDir>
#include <QTimer>
#include <QToolBar>
#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QFileDialog>

using namespace p4s;

namespace {

struct IconProvier : public QFileIconProvider {
    virtual QIcon icon(IconType) const
    {
        return QIcon();
    }
    virtual QIcon icon(const QFileInfo& info) const
    {
        static QIcon p4clIcon(":/icons/p4cl.png");

        if (info.fileName().endsWith(".p4cl")) return p4clIcon;
        return QIcon();
    }
};

struct Editor {
    evt::SplitWidget* splitter;
    evt::File* file;
    evt::FileEditor* fileEditor;
    SyntaxHighlighter* syntaxHighlighter;
    CodeAssistant* codeAssistant;
    Completer* completer;
    p4cl::compiler::Result compilationResult;
    GraphView* graphView;
};
}
struct EditView::Impl {
    QToolBox* leftWidget = new QToolBox();
    evt::FileSystemTree* fsTree = new evt::FileSystemTree();
    evt::Widget* fileSystemConainer = evt::Widget::fromContent(fsTree, leftWidget);
    evt::SplitWidget* mainWidget = new evt::SplitWidget(Qt::Horizontal);
    evt::TabWidget* tab = new evt::TabWidget(mainWidget);
    QListView* compilationLogView = new QListView(mainWidget);
    QTreeView* programTreeView = new QTreeView(mainWidget);

    CompilationLogModel* compilationLogModel = new CompilationLogModel(compilationLogView);
    ProgramTreeModel* programModel = new ProgramTreeModel(programTreeView);

    QTimer recompileTimer;

    QToolBar* cornerActions = new QToolBar(tab);

    std::unordered_map<QWidget*, Editor> editors;

public:
    Editor& currentEditor()
    {
        auto current = tab->currentWidget();
        return editors[current];
    }
    Editor& editorAt(int index)
    {
        auto current = tab->widget(index);
        return editors[current];
    }
};

EditView::EditView(QObject* parent) : QObject(parent)
{
    d->tab->setTabsClosable(true);
    d->tab->setCornerWidget(d->cornerActions);

    d->compilationLogView->setModel(d->compilationLogModel);
    d->compilationLogView->setVisible(false);

    d->programTreeView->setModel(d->programModel);
    d->programTreeView->setAlternatingRowColors(true);

    d->fsTree->openFolder(QDir::homePath());
    d->fsTree->fsModel()->setIconProvider(new IconProvier);

    d->leftWidget->addItem(d->fileSystemConainer, "Navigate");

    d->fileSystemConainer->toolbar()->addAction(
        d->mainWidget->style()->standardIcon(QStyle::SP_DirIcon), "Open directory", this,
        [this]() {
            QString dir = QFileDialog::getExistingDirectory(
                d->mainWidget, "Open working direcory", d->fsTree->rootDir(), 0);

            if (!dir.isEmpty()) {
                d->fsTree->openFolder(dir);
            }
        });

    d->recompileTimer.setInterval(700);
    d->recompileTimer.setSingleShot(true);

    connect(&d->recompileTimer, &QTimer::timeout, this, &EditView::recompile);

    createConnections();

    d->mainWidget->setSizes(QList<int>() << 1000 << 200 << 200);

    applyStyle(evt::appStyle());
}

EditView::~EditView()
{
}

void EditView::applyStyle(const evt::Style& style)
{
    d->cornerActions->setStyleSheet(style.adaptStyleSheet(evt::toolBarStyleSheet));
    d->leftWidget->setStyleSheet(style.adaptStyleSheet(evt::toolboxStyleSheet));
    d->programTreeView->setStyleSheet(style.adaptStyleSheet(evt::treeViewStyleSheet));
    d->compilationLogView->setStyleSheet(style.adaptStyleSheet(evt::listViewStyleSheet));
}

evt::MainWindowView EditView::view()
{
    return {evt::appStyle().icon(evt::IconType::Edit), "Edit", d->leftWidget,
                d->mainWidget};
}

void EditView::setWorkingDir(const QString &path)
{
    d->fsTree->openFolder(path);
}

void EditView::openFile(const QString& path)
{
    evt::File* file = new evt::File(path);

    evt::SplitWidget* splitWidget = new evt::SplitWidget(Qt::Vertical, d->tab);

    evt::FileEditor* editor = new evt::FileEditor(file, splitWidget);
    SyntaxHighlighter* syntaxHighlighter = new SyntaxHighlighter(editor->textEditor()->document());
    syntaxHighlighter->setParent(editor);
    syntaxHighlighter->applyStyle(evt::appStyle());

    file->setParent(editor);

    Editor ed;
    ed.splitter = splitWidget;
    ed.fileEditor = editor;
    ed.file = file;
    ed.syntaxHighlighter = syntaxHighlighter;

    ed.completer = new Completer(editor);
    ed.completer->completionModel()->rowHeight =
        QFontMetrics(editor->textEditor()->font()).height() * 1.5;
    auto f = editor->textEditor()->font();
    f.setBold(true);
    ed.completer->popup()->setFont(f);

    ed.codeAssistant = new CodeAssistant(editor);
    ed.codeAssistant->setCompleter(ed.completer);
    ed.graphView = new GraphView(splitWidget);
    connect(ed.graphView, &GraphView::gotoNodeRequest, this, &EditView::gotoNode);
    ed.graphView->setVisible(false);

    ed.completer->popup()->setStyleSheet(ui::completerStyleSheet());

    d->editors[splitWidget] = std::move(ed);

    splitWidget->setSizes(QList<int>() << 500 << 500);

    connect(file, &evt::File::modifiedChanged, d->tab,
            [this, file, splitWidget](bool modified) {
                auto index = d->tab->indexOf(splitWidget);
                d->tab->setTabText(index, file->fileName() + (modified ? "*" : ""));
            });

    connect(editor->textEditor(), &evt::FoldedTextEdit::textChanged, &d->recompileTimer,
            qOverload<>(&QTimer::start));

    connect(editor->textEditor(), &evt::FoldedTextEdit::fontPointSizeChanged, syntaxHighlighter,
            &SyntaxHighlighter::setFontPointSize);

    connect(editor->textEditor(), &evt::FoldedTextEdit::fontPointSizeChanged, this,
            [ this, editor, completer = ed.completer ] {
                auto f = editor->textEditor()->font();
                QFontMetrics fm(f);
                completer->completionModel()->rowHeight = fm.height() * 1.5;
            });

    auto index = d->tab->addTab(splitWidget, file->fileName());
    d->tab->setTabToolTip(index, file->path());
    d->tab->setCurrentWidget(splitWidget);

    recompile();
}

inline void addFoldingAreas(evt::FoldedTextEdit* te, auto node, auto& pr)
{
    auto pos = pr.positionOf(node);
    te->addFoldingAreaByAbsolutePosition(pos.begin, pos.end);

    for (auto n : node->children) {
        addFoldingAreas(te, n, pr);
    }
}

void EditView::recompile()
{
    auto& currentEditor = d->currentEditor();
    if (currentEditor.codeAssistant->isInCompletionMode()) return;

    auto editor = currentEditor.fileEditor;
    evt::FoldedTextEdit* te = editor->textEditor();

    auto text = te->toPlainText().toStdString();
    currentEditor.compilationResult =
        p4cl::compiler::compile(std::move(text), p4cl::lang::CompilationMode::Permissive);

    currentEditor.codeAssistant->setCompilationResult(currentEditor.compilationResult);

    if (currentEditor.graphView->isVisible()) {
        currentEditor.graphView->build(currentEditor.compilationResult);
    }

    d->programModel->setRoot(currentEditor.compilationResult.parserResult);
    d->programTreeView->expandAll();

    d->compilationLogModel->setLog(currentEditor.compilationResult.log);

    DynamicSyntaxHighlighter h;
    h.highlight(editor->textEditor(), currentEditor.compilationResult.parserResult,
                evt::appStyle());

    te->clearFoldingAreas();

    for (auto n : currentEditor.compilationResult.parserResult.rootNode->children) {
        addFoldingAreas(te, n, currentEditor.compilationResult.parserResult);
    }

    te->updateFoldingArea();

    using CompilationMessage = p4cl::lang::CompilationMessage;
    using ExtraSelection = evt::FoldedTextEdit::ExtraSelection;

    QList<ExtraSelection> warnings;
    QList<ExtraSelection> errors;

    for (const CompilationMessage& msg : *currentEditor.compilationResult.log) {
        auto position = currentEditor.compilationResult.parserResult.positionOf(msg.position);
        ExtraSelection selection = te->createExtraSelection(position.begin, position.size());
        selection.format.setUnderlineStyle(QTextCharFormat::DashUnderline);
        selection.format.setToolTip(QString::fromStdString(p4cl::toString(msg)));
        switch (msg.category) {
            case CompilationMessage::Warning: {
                selection.format.setUnderlineColor(ui::codeEditorWarningColor());
                warnings << selection;
                break;
            }
            case CompilationMessage::ParsingError:
            case CompilationMessage::CompilationError:
            case CompilationMessage::EvaluationError: {
                selection.format.setUnderlineColor(ui::codeEditorErrorColor());
                errors << selection;
                break;
            }
            default: break;
        }
    }

    te->setExtraSelections(warnings, evt::FoldedTextEdit::WarningSelection);
    te->setExtraSelections(errors, evt::FoldedTextEdit::ErrorSelection);
    te->updateExtraSelections();
}

void EditView::onCurrentTabChanged()
{
    recompile();
}

void EditView::createConnections()
{
    connect(d->tab, &evt::TabWidget::tabCloseRequested, this, &EditView::onTabCloseRequest);

    connect(d->fsTree, &evt::FileSystemTree::openFileRequest, this, &EditView::openFile);

    connect(d->tab, &evt::TabWidget::currentChanged, this, &EditView::onCurrentTabChanged);
    d->cornerActions->addAction(QIcon(":/icons/graph.png"), "Show/Hide Graph", this, [this] {
        auto& editor = d->editors[d->tab->currentWidget()];
        auto graphView = editor.graphView;
        if (!graphView->isVisible()) {
            graphView->setVisible(true);
            graphView->build(editor.compilationResult);
        }
        else {
            graphView->setVisible(false);
        }
    });

    d->cornerActions->addAction(
        evt::appStyle().icon(evt::IconType::Message), "Show/Hide compilation log", this,
        [this] { d->compilationLogView->setVisible(!d->compilationLogView->isVisible()); });

    connect(d->programTreeView, &QTreeView::doubleClicked, this,
            [this](const QModelIndex& index) { gotoNode(d->programModel->nodeAt(index)); });

    connect(d->compilationLogView, &QListView::doubleClicked, this,
            [this](const QModelIndex& index) {
                gotoPosition(d->compilationLogModel->postionOf(index));
            });
}

void EditView::gotoNode(const p4cl::ast::Node* node)
{
    auto& currentEditor = d->currentEditor();
    auto position = currentEditor.compilationResult.parserResult.positionOf(node);
    currentEditor.fileEditor->textEditor()->gotoAbsolutePosition(position.begin);
}

void EditView::gotoPosition(const p4cl::TaggedPosition& p)
{
    auto& currentEditor = d->currentEditor();
    auto position = currentEditor.compilationResult.parserResult.positionOf(p);
    currentEditor.fileEditor->textEditor()->gotoAbsolutePosition(position.begin);
}

void EditView::onTabCloseRequest(int index)
{
    auto& editor = d->editorAt(index);

    if (!editor.fileEditor->close()) {
        d->tab->removeTab(index);
        if (d->tab->count()) d->tab->setCurrentIndex(0);
    }
}
