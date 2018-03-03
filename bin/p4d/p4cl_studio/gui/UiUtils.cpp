#include "UiUtils.hpp"
#include "Theme.hpp"

#include <evt/gui/Style.hpp>
#include <evt/style/stylesheets/MainWindow.hpp>

using namespace p4s;

QString ui::nodeName(const p4cl::ast::Node* node)
{
    switch (node->nodeType) {
        case p4cl::lang::ProgramNodeType::Object: {
            const p4cl::ast::ObjectNode* n = node->as<p4cl::ast::ObjectNode>();
            if (n->id)
                return n->id->id.value.c_str();
            else
                return "<anonymous>";
        }
        case p4cl::lang::ProgramNodeType::Assignement: {
            const p4cl::ast::AssignementNode* n = node->as<p4cl::ast::AssignementNode>();
            return n->id->id.value.c_str();
        }
        default: break;
    }

    return QString("TODO");
}

QString ui::nodeType(const p4cl::ast::Node* node)
{
    switch (node->nodeType) {
        case p4cl::lang::ProgramNodeType::Object: {
            const p4cl::ast::ObjectNode* n = node->as<p4cl::ast::ObjectNode>();
            return n->type->id.value.c_str();
        }
        case p4cl::lang::ProgramNodeType::Assignement: {
            auto text = node->as<p4cl::ast::AssignementNode>()->expression->position.strView;
            return QString::fromStdString(std::string(text));
        }
        default: break;
    }

    return QString("TODO");
}

inline const p4s::Theme& customTheme()
{
    return evt::appStyle().customData<p4s::Theme>();
}

QIcon ui::nodeIcon(const p4cl::ast::Node* node)
{
    if (node->nodeType != p4cl::lang::ProgramNodeType::Object) return QIcon();

    auto typeLabel = node->as<p4cl::ast::ObjectNode>()->type->position.strView;
    if (p4cl::lang::findDataType(typeLabel)) {
        return customTheme().lang.dataIcon;
    }
    else if (auto task = p4cl::lang::findTaskType(typeLabel)) {
        if (task.category() == p4cl::lang::TaskCategory::Processing)
            return customTheme().lang.processingIcon;
        else if (task.category() == p4cl::lang::TaskCategory::Export)
            return customTheme().lang.exportIcon;
    }

    return QIcon();
}

QIcon ui::dataIcon(IconShape s)
{
    if (s == IconShape::Rectangular) return customTheme().lang.dataIcon;
    return customTheme().lang.dataIconRound;
}
QIcon ui::processingIcon(IconShape s)
{
    if (s == IconShape::Rectangular) return customTheme().lang.processingIcon;
    return customTheme().lang.processingIconRound;
}
QIcon ui::exportIcon(IconShape s)
{
    if (s == IconShape::Rectangular) return customTheme().lang.exportIcon;
    return customTheme().lang.exportIconRound;
}

QColor ui::nodeColor(const p4cl::compiler::Node* node)
{
    if (node->astNode->nodeType != p4cl::lang::ProgramNodeType::Object) return QColor();

    auto typeLabel = node->astNode->as<p4cl::ast::ObjectNode>()->type->position.strView;
    if (p4cl::lang::findDataType(typeLabel)) {
        return dataColor();
    }
    else if (auto task = p4cl::lang::findTaskType(typeLabel)) {
        if (task.category() == p4cl::lang::TaskCategory::Processing)
            return processingColor();
        else if (task.category() == p4cl::lang::TaskCategory::Export)
            return exportColor();
    }

    return QColor();
}

QColor ui::dataColor()
{
    return customTheme().lang.dataColor;
}

QColor ui::processingColor()
{
    return customTheme().lang.processingColor;
}

QColor ui::exportColor()
{
    return customTheme().lang.exportColor;
}

QColor ui::codeEditorTextColor()
{
    return customTheme().textEdit.textColor;
}

QFont ui::codeEditorFont()
{
    return customTheme().textEdit.font;
}

QColor ui::codeEditorHighlightColor()
{
    return customTheme().textEdit.highlightColor;
}

QColor ui::codeEditorOutlineColor()
{
    return customTheme().textEdit.outlineColor;
}

QColor ui::codeEditorCommentColor()
{
    return customTheme().textEdit.commentColor;
}

QColor ui::codeEditorKeywordColor()
{
    return customTheme().lang.keywordColor;
}

QColor ui::codeEditorJsKeywordColor()
{
    return customTheme().lang.jsKeywordColor;
}

QColor ui::codeEditorLiteralColor()
{
    return customTheme().textEdit.literalColor;
}

QColor ui::codeEditorObjectColor()
{
    return customTheme().lang.scriptObjectColor;
}

QColor ui::codeEditorPropertyColor()
{
    return customTheme().lang.propertyColor;
}

QColor ui::codeEditorWarningColor()
{
    return customTheme().textEdit.warningColor;
}

QColor ui::codeEditorErrorColor()
{
    return customTheme().textEdit.errorColor;
}

QString ui::completerStyleSheet()
{
    return evt::appStyle().adaptStyleSheet(evt::scrollBarStyleSheet);
}
