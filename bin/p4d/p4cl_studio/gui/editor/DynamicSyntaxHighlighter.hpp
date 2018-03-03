#pragma once

namespace evt {
class FoldedTextEdit;
class Style;
}
namespace p4cl {
namespace parser {
class Result;
}
}

namespace p4s {
class DynamicSyntaxHighlighter {
    using ParserResult = p4cl::parser::Result;

public:
    DynamicSyntaxHighlighter();
    void highlight(evt::FoldedTextEdit*, const ParserResult&, const evt::Style&);

protected:
    void highlightObjects(evt::FoldedTextEdit*, const ParserResult&, const evt::Style&);
    void highlightProperties(evt::FoldedTextEdit*, const ParserResult&, const evt::Style&);
};
}
