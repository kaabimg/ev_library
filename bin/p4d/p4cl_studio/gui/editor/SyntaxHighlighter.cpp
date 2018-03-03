#include "SyntaxHighlighter.hpp"
#include "../UiUtils.hpp"

#include <p4cl/P4CLang.hpp>

#include <QRegularExpression>

using namespace p4s;

namespace {

struct HighlightingRule {
    enum TextFormat { NoFormat = 0, Italic = 1, Bold = 1 << 2 };

    static HighlightingRule create(const QString& pattern,
                                   const QFont& font,
                                   const QColor& color,
                                   TextFormat format = NoFormat)
    {
        HighlightingRule rule;
        rule.pattern.setPattern(pattern);
        rule.format.setForeground(color);
        QFont f = font;
        f.setItalic(format & Italic);
        f.setBold(format & Bold);
        rule.format.setFont(f);
        return rule;
    }

    QRegularExpression pattern;
    QTextCharFormat format;
};
}

struct SyntaxHighlighter::Impl {
    void setStyle(const evt::Style& style)
    {
        rules.clear();

        for (auto& t : p4cl::lang::supportedTasks) {
            if (t.category() == p4cl::lang::TaskCategory::Processing) {
                rules << HighlightingRule::create("\\b" + QString(t.label) + "\\b",
                                                  ui::codeEditorFont(), ui::processingColor(),
                                                  HighlightingRule::Bold);
            }
            else if (t.category() == p4cl::lang::TaskCategory::Export) {
                rules << HighlightingRule::create("\\b" + QString(t.label) + "\\b",
                                                  ui::codeEditorFont(), ui::exportColor(),
                                                  HighlightingRule::Bold);
            }
        }

        for (auto& t : p4cl::lang::supportedData) {
            rules << HighlightingRule::create("\\b" + QString(t.label) + "\\b",
                                              ui::codeEditorFont(), ui::dataColor(),
                                              HighlightingRule::Bold);
        }

        for (auto& t : p4cl::lang::scriptTypes) {
            rules << HighlightingRule::create("\\b" + QString(t.c_str()) + "\\b",
                                              ui::codeEditorFont(), ui::processingColor(),
                                              HighlightingRule::Bold);
        }

        for (const char* kw : p4cl::lang::endKeyWords) {
            rules << HighlightingRule::create("\\b" + QString(kw) + "\\b", ui::codeEditorFont(),
                                              ui::codeEditorKeywordColor(), HighlightingRule::Bold);
        }

        for (const char* kw : p4cl::lang::p4clKeyWords) {
            rules << HighlightingRule::create("\\b" + QString(kw) + "\\b", ui::codeEditorFont(),
                                              ui::codeEditorKeywordColor(),
                                              HighlightingRule::NoFormat);
        }

        for (const char* kw : p4cl::lang::jsKeyWords) {
            rules << HighlightingRule::create("\\b" + QString(kw) + "\\b", ui::codeEditorFont(),
                                              ui::codeEditorJsKeywordColor(),
                                              HighlightingRule::NoFormat);
        }

        rules << HighlightingRule::create(
            "\\b[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?([aAfFpPnNuUgGtTkKmM])?"
            "\\b",
            ui::codeEditorFont(), ui::codeEditorLiteralColor());

        // strings
        rules << HighlightingRule::create("\"[^\"]*\"", ui::codeEditorFont(),
                                          ui::codeEditorLiteralColor());

        rules << HighlightingRule::create("'[^']*'", ui::codeEditorFont(),
                                          ui::codeEditorLiteralColor());

        rules << HighlightingRule::create("#[^\n]*", ui::codeEditorFont(),
                                          ui::codeEditorCommentColor());
    }

    QVector<HighlightingRule> rules;
};

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* document) : QSyntaxHighlighter(document)
{
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightingRule& rule : _impl->rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
}

void SyntaxHighlighter::applyStyle(const evt::Style& style)
{
    _impl->setStyle(style);
    rehighlight();
}

void SyntaxHighlighter::setFontPointSize(int size)
{
    for (auto& rule : _impl->rules) rule.format.setFontPointSize(size);
    rehighlight();
}
