#pragma once

#include <ev/core/pimpl.hpp>
#include <QObject>

namespace evt {
class FileEditor;
}

namespace p4cl {
namespace compiler {
class Result;
}
}
namespace p4s {
class Completer;

class CodeAssistant : public QObject {
public:
    CodeAssistant(evt::FileEditor*);
    ~CodeAssistant();
    void setCompleter(Completer*);
    bool isInCompletionMode() const;

    void setCompilationResult(p4cl::compiler::Result);

protected:
    QString currentLineIndent() const;

private:
    void onEnterPressed();
    void onCompleteRequest();
    void onEscape();
    void insertCompletion();
    void onCursorPositionChanged();
    QString textUnderCursor() const;
    void closeCompleter();
    void newLine();
    void generateCompletion();

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
