#pragma once

#include <QStringRef>

namespace evt {

struct CharPosition
{
    uint32_t line   = 0;
    uint32_t column = 0;

    CharPosition() {}
    CharPosition(const CharPosition& o) : line(o.line), column(o.column) {}
    CharPosition(uint l, uint c) : line(l), column(c) {}
    bool isValid() const { return line && column; }
    bool operator==(const CharPosition& another) const
    {
        return another.line == line && another.column == column;
    }

    bool operator<(const CharPosition& another) const
    {
        return line < another.line ||
               (line == another.line && column < another.column);
    }
    bool operator<=(const CharPosition& another) const
    {
        return line < another.line ||
               (another.line == line && column <= another.column);
    }

    bool operator>(const CharPosition& another) const
    {
        return line > another.line ||
               (another.line == line && column > another.column);
    }

    bool operator>=(const CharPosition& another) const
    {
        return line > another.line ||
               (another.line == line && column >= another.column);
    }
};

struct TextPosition
{
    TextPosition() { startIndex = endIndex = 0; }
    TextPosition(uint sl, uint sc, uint el, uint ec)
        : start(sl, sc), end(el, ec)
    {
        startIndex = endIndex = 0;
    }
    TextPosition(CharPosition s, CharPosition e) : start(s), end(e)
    {
        startIndex = endIndex = 0;
    }

    bool isValid() const { return start.isValid() && end.isValid(); }
    bool contains(uint line, uint column) const
    {
        CharPosition pos(line, column);
        return start <= pos && pos <= end;
    }

    QStringRef stringRef(const QString* str) const
    {
        return QStringRef(str, startIndex, endIndex - startIndex + 1);
    }

    int size() const { return endIndex - startIndex; }
    static TextPosition merge(const TextPosition& start,
                                const TextPosition& end)
    {
        TextPosition postion;
        postion.start      = start.start;
        postion.startIndex = start.startIndex;
        postion.end        = end.end;
        postion.endIndex   = end.endIndex;
        return postion;
    }

    ///
    CharPosition start, end;
    uint startIndex, endIndex;
};

}

