#include "CodeAssistant.hpp"
#include "Completer.hpp"
#include "CompletionModel.hpp"

#include <p4cl/compiler/Compiler.hpp>
#include <evt/gui/FileEditor.hpp>
#include <evt/gui/textedit/FoldedTextEdit.hpp>

#include <QTextBlock>
#include <QAbstractItemView>
#include <QScrollBar>

#include <QDebug>

using namespace p4s;

struct CodeAssistant::Impl {
    evt::FoldedTextEdit* editor;
    Completer* completer = nullptr;
    p4cl::compiler::Result compilationResult;
};

CodeAssistant::CodeAssistant(evt::FileEditor* editor) : QObject(editor)
{
    d->editor = editor->textEditor();

    d->editor->monitorSequence(Qt::Key_Enter, [this] { onEnterPressed(); });
    d->editor->monitorSequence(Qt::Key_Return, [this] { onEnterPressed(); });

    d->editor->monitorSequence(QKeySequence(Qt::ControlModifier + Qt::Key_Space),
                                   [this] { onCompleteRequest(); });
    d->editor->monitorSequence(Qt::Key_Escape, [this] { onEscape(); });

    connect(editor->textEditor(), &evt::FoldedTextEdit::cursorPositionChanged, this,
            &CodeAssistant::onCursorPositionChanged);
}

CodeAssistant::~CodeAssistant()
{
}

void CodeAssistant::setCompleter(Completer* c)
{
    d->completer = c;
}

void CodeAssistant::onEnterPressed()
{
    if (isInCompletionMode()) {
        insertCompletion();
    }
    else {
        newLine();
    }
}

bool CodeAssistant::isInCompletionMode() const
{
    return d->completer->popup()->isVisible();
}

void CodeAssistant::setCompilationResult(p4cl::compiler::Result r)
{
    d->compilationResult = r;
}

void CodeAssistant::closeCompleter()
{
    d->completer->popup()->hide();
}

QString CodeAssistant::currentLineIndent() const
{
    QTextCursor cursor = d->editor->textCursor();

    QString text = cursor.block().text();

    text.remove(cursor.positionInBlock(), text.size() - cursor.positionInBlock());
    QString indent;

    for (QChar c : text) {
        if (c.isSpace())
            indent.append(c);
        else
            break;
    }

    text = text.trimmed();
    if (!text.isEmpty()) {
        switch (text[text.size() - 1].toLatin1()) {
            case ':':
            case '{':
            case '(': indent.append("\t"); break;
            default: break;
        }
    }

    return indent;
}

void CodeAssistant::newLine()
{
    QTextCursor cursor = d->editor->textCursor();
    QString i = currentLineIndent();
    cursor.insertText("\n");
    cursor.insertText(i);
}

namespace {
const p4cl::compiler::Node* findContainer(const p4cl::parser::Result& pr,
                                          const p4cl::compiler::Node* root,
                                          int p)
{
    if (root->astNode && !pr.positionOf(root->astNode).contains(p)) return nullptr;

    for (auto child : root->children) {
        auto position = pr.positionOf(child->astNode);
        if (position.contains(p)) return findContainer(pr, child, p);
    }

    return root;
}
}

void CodeAssistant::generateCompletion()
{
    if (!d->compilationResult.program) return;

    QTextCursor cursor = d->editor->textCursor();
    const char* cursorPosition =
        d->compilationResult.parserResult.code->c_str() + cursor.position();
    auto container = findContainer(d->compilationResult.parserResult,
                                   d->compilationResult.program.get(), cursor.position());

    std::string_view typeName;
    if (container == nullptr || container == d->compilationResult.program.get()) {
        typeName = p4cl::lang::programTypeName;
    }
    else {
        if (auto task = qobject_cast<const p4cl::compiler::TaskNode*>(container)) {
            typeName = task->astNode->as<p4cl::ast::ObjectNode>()->type->position.strView;
        }
        else if (auto data = qobject_cast<const p4cl::compiler::DataNode*>(container)) {
            typeName = data->astNode->as<p4cl::ast::ObjectNode>()->type->position.strView;
        }
    }

    std::vector<AssistItem> assitItems;

    /// generate insertable types

    auto insertableTypes = p4cl::lang::findTypeNestedTypes(typeName);
    if (insertableTypes) {
        for (auto& t : insertableTypes.subTypes) {
            auto typeProperties = p4cl::lang::findTypeProperties(t);
            if (typeProperties) {
                assitItems.push_back(AssistItem::fromType(typeProperties));
            }
        }
    }

    /// generate insertable properties

    auto typeProperties = p4cl::lang::findTypeProperties(typeName);
    if (typeProperties) {
        for (auto& p : typeProperties.properties) {
            assitItems.push_back(AssistItem::fromProperty(p));
        }
    }

    /// generate created variables names
    if (container) {
        /// name, type
        std::vector<std::pair<std::string_view, std::string_view>> variables;
        auto visitor = [&variables, cursorPosition](const p4cl::compiler::Node* node) {
            if (node->astNode && node->astNode->position.strView.begin() < cursorPosition) {
                const p4cl::ast::ObjectNode* objNode = nullptr;
                if (auto task = qobject_cast<const p4cl::compiler::TaskNode*>(node)) {
                    objNode = task->astNode->as<p4cl::ast::ObjectNode>();
                }
                else if (auto data = qobject_cast<const p4cl::compiler::DataNode*>(node)) {
                    objNode = data->astNode->as<p4cl::ast::ObjectNode>();
                }

                if (objNode && objNode->id) {
                    variables.push_back(
                        {objNode->id->position.strView, objNode->type->position.strView});
                }
            }
        };

        d->compilationResult.program->applyVisitor(visitor);

        for (const auto& var : variables) {
            assitItems.push_back(AssistItem::fromVariable(var));
        }
    }

    d->completer->completionModel()->updateAssistItems(std::move(assitItems));
}

void CodeAssistant::onCompleteRequest()
{
    generateCompletion();
    QRect rect = d->editor->cursorRect();
    rect.setWidth(300);

    rect.translate(d->editor->linesNumbersAreaWidth() + d->editor->foldingAreaWidth(),
                   d->editor->statusWidgetHeight() + 5);

    d->completer->setCompletionPrefix(textUnderCursor());

    d->completer->complete(rect);
}

void CodeAssistant::onCursorPositionChanged()
{
    if (isInCompletionMode()) {
        onCompleteRequest();
    }
}

void CodeAssistant::onEscape()
{
    closeCompleter();
}

void CodeAssistant::insertCompletion()
{
    QString prefix = textUnderCursor();

    QTextCursor cursor = d->editor->textCursor();

    closeCompleter();

    for (int i = 0; i < prefix.size(); ++i) {
        cursor.deletePreviousChar();
    }

    int position = cursor.position();

    int completionIndex =
        d->completer->popup()->selectionModel()->currentIndex().data(Qt::EditRole).toInt();

    const auto& item = d->completer->completionModel()->assistItems[completionIndex];

    auto completion = item.generate(currentLineIndent());

    cursor.insertText(completion.string);

    if (completion.substitution.isValid()) {
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor,
                            cursor.position() - position - completion.substitution.begin);

        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                            completion.substitution.size());
    }

    d->editor->setTextCursor(cursor);
}

QString CodeAssistant::textUnderCursor() const
{
    QTextCursor cursor = d->editor->textCursor();

    QString text;

    int parentherisCount = 0;
    int bracketsCount = 0;
    bool isInDoubleQuoteString = false;
    bool isInSimpleQuoteString = false;

    bool startedWithSpace = false;
    bool firstChar = true;

    bool waitingForADot = false;

    while (cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1)) {
        text = cursor.selectedText();

        if (firstChar) {
            firstChar = false;
            if (text[0].isSpace()) {
                startedWithSpace = true;
            }
        }
        else {
            if (text[0] != '.' && !text[0].isSpace() && startedWithSpace) {
                return "";
            }
        }

        if (text[0].isSpace()) {
            waitingForADot = true;
            continue;
        }

        if (waitingForADot && text[0] != '.') {
            text.remove(0, 1);
            return text.trimmed();
        }

        switch (text[0].toLatin1()) {
            case '"': {
                isInDoubleQuoteString = !isInDoubleQuoteString;
                break;
            }
            case '\'': {
                isInSimpleQuoteString = !isInSimpleQuoteString;
                break;
            }

            break;
            default: break;
        }

        if (isInDoubleQuoteString || isInSimpleQuoteString) {
            continue;
        }

        switch (text[0].toLatin1()) {
            case '/':
            case '[': {
                if (bracketsCount) {
                    --bracketsCount;
                    break;
                }
                else {
                    text.remove(0, 1);
                    return text.trimmed();
                }
            }
            case ']': {
                ++bracketsCount;
                break;
            }
            case '(': {
                if (parentherisCount) {
                    --parentherisCount;
                    break;
                }
                else {
                    text.remove(0, 1);
                    return text.trimmed();
                }
            }
            case ')': {
                ++parentherisCount;
                break;
            }
            case '{':
            case '}':
            case ';':
            case ',':
            case ':':
            case '!':
            case '+':
            case '-':
            case '*':
            case '=':
            case '~':
            case '&':
            case '|': {
                if (!parentherisCount && !bracketsCount) {
                    text.remove(0, 1);
                    return text.trimmed();
                }
                break;
            }
            default: break;
        }
    }
    return text.trimmed();
}
