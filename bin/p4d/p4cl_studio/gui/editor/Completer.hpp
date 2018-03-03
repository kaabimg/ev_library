#pragma once
#include <QCompleter>

#include <ev/core/pimpl.hpp>
namespace evt {
class FileEditor;
}

namespace p4cl {
namespace parser {
class Result;
}
}

namespace p4s {

class CompletionModel;

class Completer : public QCompleter {
public:
    Completer(evt::FileEditor*);
    ~Completer();

    CompletionModel* completionModel() const;

private:
    class Impl;
    ev::pimpl<Impl> _impl;
};
}
