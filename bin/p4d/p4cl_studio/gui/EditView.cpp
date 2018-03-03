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
    _impl->tab->setTabsClosable(true);
    _impl->tab->setCornerWidget(_impl->cornerActions);

    _impl->compilationLogView->setModel(_impl->compilationLogModel);
    _impl->compilationLogView->setVisible(false);

    _impl->programTreeView->setModel(_impl->programModel);
    _impl->programTreeView->setAlternatingRowColors(true);

    _impl->fsTree->openFolder(QDir::homePath());
    _impl->fsTree->fsModel()->setIconProvider(new IconProvier);

    _impl->leftWidget->addItem(_impl->fileSystemConainer, "Navigate");

    _impl->fileSystemConainer->toolbar()->addAction(
        _impl->mainWidget->style()->standardIcon(QStyle::SP_DirIcon), "Open directory", this,
        [this]() {
            QString dir = QFileDialog::getExistingDirectory(
                _impl->mainWidget, "Open working direcory", _impl->fsTree->rootDir(), 0);

            if (!dir.isEmpty()) {
                _impl->fsTree->openFolder(dir);
            }
        });

    _impl->recompileTimer.setInterval(700);
    _impl->recompileTimer.setSingleShot(true);

    connect(&_impl->recompileTimer, &QTimer::timeout, this, &EditView::recompile);

    createConnections();

    _impl->mainWidget->setSizes(QList<int>() << 1000 << 200 << 200);

    applyStyle(evt::appStyle());
}

EditView::~EditView()
{
}

void EditView::applyStyle(const evt::Style& style)
{
    _impl->cornerActions->setStyleSheet(style.adaptStyleSheet(evt::toolBarStyleSheet));
    _impl->leftWidget->setStyleSheet(style.adaptStyleSheet(evt::toolboxStyleSheet));
    _impl->programTreeView->setStyleSheet(style.adaptStyleSheet(evt::treeViewStyleSheet));
    _impl->compilationLogView->setStyleSheet(style.adaptStyleSheet(evt::listViewStyleSheet));
}

evt::MainWindowView EditView::view()
{
    return {evt::appStyle().icon(evt::IconType::Edit), "Edit", _impl->leftWidget,
                _impl->mainWidget};
}

void EditView::setWorkingDir(const QString &path)
{
    _impl->fsTree->openFolder(path);
}

void EditView::openFile(const QString& path)
{
    evt::File* file = new evt::File(path);

    evt::SplitWidget* splitWidget = new evt::SplitWidget(Qt::Vertical, _impl->tab);

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

    _impl->editors[splitWidget] = std::move(ed);

    splitWidget->setSizes(QList<int>() << 500 << 500);

    connect(file, &evt::File::modifiedChanged, _impl->tab,
            [this, file, splitWidget](bool modified) {
                auto index = _impl->tab->indexOf(splitWidget);
                _impl->tab->setTabText(index, file->fileName() + (modified ? "*" : ""));
            });

    connect(editor->textEditor(), &evt::FoldedTextEdit::textChanged, &_impl->recompileTimer,
            qOverload<>(&QTimer::start));

    connect(editor->textEditor(), &evt::FoldedTextEdit::fontPointSizeChanged, syntaxHighlighter,
            &SyntaxHighlighter::setFontPointSize);

    connect(editor->textEditor(), &evt::FoldedTextEdit::fontPointSizeChanged, this,
            [ this, editor, completer = ed.completer ] {
                auto f = editor->textEditor()->font();
                QFontMetrics fm(f);
                completer->completionModel()->rowHeight = fm.height() * 1.5;
            });

    auto index = _impl->tab->addTab(splitWidget, file->fileName());
    _impl->tab->setTabToolTip(index, file->path());
    _impl->tab->setCurrentWidget(splitWidget);

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
    auto& currentEditor = _impl->currentEditor();
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

    _impl->programModel->setRoot(currentEditor.compilationResult.parserResult);
    _impl->programTreeView->expandAll();

    _impl->compilationLogModel->setLog(currentEditor.compilationResult.log);

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
    connect(_impl->tab, &evt::TabWidget::tabCloseRequested, this, &EditView::onTabCloseRequest);

    connect(_impl->fsTree, &evt::FileSystemTree::openFileRequest, this, &EditView::openFile);

    connect(_impl->tab, &evt::TabWidget::currentChanged, this, &EditView::onCurrentTabChanged);
    _impl->cornerActions->addAction(QIcon(":/icons/graph.png"), "Show/Hide Graph", this, [this] {
        auto& editor = _impl->editors[_impl->tab->currentWidget()];
        auto graphView = editor.graphView;
        if (!graphView->isVisible()) {
            graphView->setVisible(true);
            graphView->build(editor.compilationResult);
        }
        else {
            graphView->setVisible(false);
        }
    });

    _impl->cornerActions->addAction(
        evt::appStyle().icon(evt::IconType::Message), "Show/Hide compilation log", this,
        [this] { _impl->compilationLogView->setVisible(!_impl->compilationLogView->isVisible()); });

    connect(_impl->programTreeView, &QTreeView::doubleClicked, this,
            [this](const QModelIndex& index) { gotoNode(_impl->programModel->nodeAt(index)); });

    connect(_impl->compilationLogView, &QListView::doubleClicked, this,
            [this](const QModelIndex& index) {
                gotoPosition(_impl->compilationLogModel->postionOf(index));
            });
}

void EditView::gotoNode(const p4cl::ast::Node* node)
{
    auto& currentEditor = _impl->currentEditor();
    auto position = currentEditor.compilationResult.parserResult.positionOf(node);
    currentEditor.fileEditor->textEditor()->gotoAbsolutePosition(position.begin);
}

void EditView::gotoPosition(const p4cl::TaggedPosition& p)
{
    auto& currentEditor = _impl->currentEditor();
    auto position = currentEditor.compilationResult.parserResult.positionOf(p);
    currentEditor.fileEditor->textEditor()->gotoAbsolutePosition(position.begin);
}

void EditView::onTabCloseRequest(int index)
{
    auto& editor = _impl->editorAt(index);

    if (!editor.fileEditor->close()) {
        _impl->tab->removeTab(index);
        if (_impl->tab->count()) _impl->tab->setCurrentIndex(0);
    }
}
