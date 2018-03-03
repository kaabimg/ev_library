#pragma once

#include <evt/core/Logging.hpp>

#include <QQmlEngine>
#include <QString>

namespace evt {

class ApplicationScriptEngine : public QObject {
    Q_OBJECT
public:
    ApplicationScriptEngine(QObject* parent = nullptr);

    void registerObject(const QString& name,QObject*);

public Q_SLOTS:
    void evaluate(const QString&);

Q_SIGNALS:
    void resultReady(QString, LogMessage);

private:
    QString evaluateCode(const QString&);

private:
    QQmlEngine _engine;
};
}

