#pragma once

#include <ev/core/pimpl.hpp>

#include <QSyntaxHighlighter>

namespace evt {
class Style;
}

namespace p4s {

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    enum State { Code, Comment };

    SyntaxHighlighter(QTextDocument*);
    ~SyntaxHighlighter();

    void highlightBlock(const QString& text);

    void applyStyle(const evt::Style&);

public Q_SLOTS:
    void setFontPointSize(int);

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
