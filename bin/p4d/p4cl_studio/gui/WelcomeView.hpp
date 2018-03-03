#pragma once

#include <ev/core/pimpl.hpp>
#include <evt/gui/MainWindow.hpp>

class QWidget;
namespace evt {
class FileEditor;
class Style;
}
namespace p4s {

class WelcomeView : public QObject {
    Q_OBJECT
public:
    WelcomeView(QObject* parent = nullptr);
    ~WelcomeView();
    void applyStyle(const evt::Style&);

    evt::MainWindowView view();

private:
    class Impl;
    ev::pimpl<Impl> _impl;
};
}
