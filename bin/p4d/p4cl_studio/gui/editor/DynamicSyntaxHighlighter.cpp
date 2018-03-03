#include "DynamicSyntaxHighlighter.hpp"
#include "../UiUtils.hpp"
#include <evt/gui/textedit/FoldedTextEdit.hpp>
#include <p4cl/parser/Parser.hpp>

#include <QDebug>

using namespace p4s;

using ExtraSelection = evt::FoldedTextEdit::ExtraSelection;

DynamicSyntaxHighlighter::DynamicSyntaxHighlighter()
{
}

void DynamicSyntaxHighlighter::highlight(evt::FoldedTextEdit* textEdit,
                                         const ParserResult& parserResult,
                                         const evt::Style& style)
{
    if (!parserResult.isValid()) return;

    textEdit->clearExtraSelections(evt::FoldedTextEdit::SyntaxHighlightSelection);
    textEdit->clearExtraSelections(evt::FoldedTextEdit::ErrorSelection);

    highlightObjects(textEdit, parserResult, style);
    highlightProperties(textEdit, parserResult, style);
    textEdit->updateExtraSelections();
}

inline void _highlightObjects(QList<ExtraSelection>& selectionList,
                              const p4cl::parser::Result& result,
                              const p4cl::ast::Node* node,
                              evt::FoldedTextEdit* textEdit)
{
    if (node->nodeType == p4cl::lang::ProgramNodeType::Object) {
        auto objectNode = node->as<p4cl::ast::ObjectNode>();
        if (objectNode->id) {
            auto position = result.positionOf(objectNode->id.get());
            if (position.isValid()) {
                ExtraSelection selection;
                selection = textEdit->createExtraSelection(position.begin, position.size());
                selectionList << selection;
            }
        }
    }

    for (auto c : node->children) {
        _highlightObjects(selectionList, result, c, textEdit);
    }
}

void DynamicSyntaxHighlighter::highlightObjects(evt::FoldedTextEdit* textEdit,
                                                const ParserResult& result,
                                                const evt::Style& style)
{
    QList<ExtraSelection> selectionList;
    _highlightObjects(selectionList, result, result.rootNode.get(), textEdit);

    for (auto& selection : selectionList) {
        selection.format.setForeground(ui::codeEditorTextColor());
    }

    textEdit->addExtraSelections(selectionList, evt::FoldedTextEdit::SyntaxHighlightSelection);
}

inline void _highlightProperties(QList<ExtraSelection>& selectionList,
                                 const p4cl::parser::Result& result,
                                 const p4cl::ast::Node* node,
                                 evt::FoldedTextEdit* textEdit)
{
    if (node->nodeType == p4cl::lang::ProgramNodeType::Assignement) {
        auto position = result.positionOf(node->as<p4cl::ast::AssignementNode>()->id.get());
        if (position.isValid()) {
            ExtraSelection selection;
            selection = textEdit->createExtraSelection(position.begin, position.size());
            selectionList << selection;
        }
    }

    for (auto c : node->children) {
        _highlightProperties(selectionList, result, c, textEdit);
    }
}

void DynamicSyntaxHighlighter::highlightProperties(evt::FoldedTextEdit* textEdit,
                                                   const ParserResult& result,
                                                   const evt::Style& style)
{
    QList<ExtraSelection> selectionList;
    _highlightProperties(selectionList, result, result.rootNode.get(), textEdit);

    for (auto& selection : selectionList) {
        selection.format.setForeground(ui::codeEditorPropertyColor());
    }

    textEdit->addExtraSelections(selectionList, evt::FoldedTextEdit::SyntaxHighlightSelection);
}
