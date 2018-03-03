#pragma once

#include <ev/core/pimpl.hpp>

#include <QMainWindow>

namespace evt {

class Style;

struct MainWindowView {
    QIcon icon;
    QString label;
    QWidget* left;
    QWidget* central;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    void applyStyle(const Style&);

    void addView(const MainWindowView&);
    void setCurrentView(uint id);

private:
    struct Impl;
    ev::pimpl<Impl> d;
};
}
