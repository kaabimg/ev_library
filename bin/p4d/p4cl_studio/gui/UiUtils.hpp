#pragma once
#include <p4cl/compiler/Compiler.hpp>

namespace p4s {
namespace ui {

QColor dataColor();
QColor processingColor();
QColor exportColor();

QFont codeEditorFont();
QColor codeEditorTextColor();

QColor codeEditorHighlightColor();
QColor codeEditorOutlineColor();
QColor codeEditorCommentColor();

QColor codeEditorKeywordColor();
QColor codeEditorJsKeywordColor();
QColor codeEditorLiteralColor();

QColor codeEditorObjectColor();
QColor codeEditorPropertyColor();

QColor codeEditorWarningColor();
QColor codeEditorErrorColor();

QColor nodeColor(const p4cl::compiler::Node*);
QString nodeName(const p4cl::ast::Node* node);
QString nodeType(const p4cl::ast::Node* node);
QIcon nodeIcon(const p4cl::ast::Node* node);

enum class IconShape{Round, Rectangular};

QIcon dataIcon(IconShape s= IconShape::Rectangular);
QIcon processingIcon(IconShape s= IconShape::Rectangular);
QIcon exportIcon(IconShape s= IconShape::Rectangular);

QString completerStyleSheet();

}
}
