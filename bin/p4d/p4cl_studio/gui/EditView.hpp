#pragma once

#include <ev/core/pimpl.hpp>
#include <evt/gui/MainWindow.hpp>

class QWidget;
namespace evt {
class FileEditor;
class Style;
}

namespace p4cl {
class TaggedPosition;

namespace ast {
class Node;
}
}

namespace p4s {

class EditView : public QObject {
    Q_OBJECT
public:
    EditView(QObject* parent = nullptr);
    ~EditView();

    void applyStyle(const evt::Style&);

    evt::MainWindowView view();

public Q_SLOTS:
    void setWorkingDir(const QString& path);
    void openFile(const QString& path);

private Q_SLOTS:
    void recompile();
    void onCurrentTabChanged();

private:
    void createConnections();
    void gotoNode(const p4cl::ast::Node*);
    void gotoPosition(const p4cl::TaggedPosition &);
    void onTabCloseRequest(int);

private:
    class Impl;
    ev::pimpl<Impl> _impl;
};
}
