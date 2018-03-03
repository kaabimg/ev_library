#include "ApplicationScriptEngine.hpp"

#include <QJsonDocument>
#include <QQmlContext>

evt::ApplicationScriptEngine::ApplicationScriptEngine(QObject* parent) : QObject(parent)
{
    _engine.installExtensions(QJSEngine::AllExtensions);
}

void evt::ApplicationScriptEngine::registerObject(const QString& name, QObject* obj)
{
    QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
    _engine.globalObject().setProperty(name, _engine.newQObject(obj));
}

void evt::ApplicationScriptEngine::evaluate(const QString& code)
{
    QString result = evaluateCode(code);
    Q_EMIT resultReady(code, {result.toStdString(), LogCategory::Debug});
}

QString evt::ApplicationScriptEngine::evaluateCode(const QString& code)
{
    QJSValue val = _engine.evaluate(code);
    if (val.isString()) {
        return "'" + val.toString() + "'";
    }
    else if (val.isBool() || val.isDate() || val.isNull() || val.isNumber() || val.isError()) {
        return val.toString();
    }
    else if (val.isCallable()) {
        return "function";
    }
    else if (val.isQObject()) {
        return "object " + val.toQObject()->objectName();
    }
    else {
        return QJsonDocument::fromVariant(val.toVariant()).toJson(QJsonDocument::Compact);
    }
}
