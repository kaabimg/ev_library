#include "Completer.hpp"

#include <p4cl/parser/Parser.hpp>
#include <evt/gui/FileEditor.hpp>
#include <evt/gui/textedit/FoldedTextEdit.hpp>

#include "CompletionModel.hpp"

using namespace p4s;

struct Completer::Impl {
    evt::FoldedTextEdit* editor;
};

Completer::Completer(evt::FileEditor* editor) : QCompleter(editor)
{
    _impl->editor = editor->textEditor();

    setCompletionMode(PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionRole(Qt::DisplayRole);

    setModel(new CompletionModel(editor));

    setWidget(editor->textEditor());
}

Completer::~Completer()
{
}

CompletionModel* Completer::completionModel() const
{
    return reinterpret_cast<CompletionModel*>(model());
}
