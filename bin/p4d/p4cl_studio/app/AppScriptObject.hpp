#pragma once

#include <QObject>

namespace p4s {
class EditView;
class AppScriptObject : public QObject {
    Q_OBJECT
public:
    AppScriptObject(QObject* parent = nullptr) : QObject(parent)
    {
    }

public Q_SLOTS:
    QString set_working_dir(const QString&);

public:
    EditView* editView;
};
}
