#pragma once

#include <ev/core/pimpl.hpp>

#include <QWidget>

namespace evt {
class Style;
class LogMessage;

class Console : public QWidget {
    Q_OBJECT
public:
    explicit Console(QWidget* parent = nullptr);
    ~Console();

    void applyStyle(const Style&);

Q_SIGNALS:
    void execRequest(QString);

public Q_SLOTS:
    void addOutput(const QString& cmd, const LogMessage& msg);

protected:
    bool eventFilter(QObject* target, QEvent* event);

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
