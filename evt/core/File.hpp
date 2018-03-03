#pragma once

#include <ev/core/pimpl.hpp>
#include <QObject>

namespace evt {

class File : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
public:
    explicit File(const QString& path = "", QObject* parent = 0);
    ~File();
    void setPath(const QString& path);
    bool isValid() const;

    QString fileName() const;
    QString path() const;

    QString dir() const;
    bool exists() const;
    bool rename(const QString& name);

    QDateTime lastModified() const;

    static bool save(const QString& data, const QString& path);
    bool save(const QString& data);
    QString readContent(bool* ok = NULL);
    QString errorString() const;
    bool modified() const;

public Q_SLOTS:
    void watch();
    void unwatch();
    void rewatch();
    void setModified(bool modified);

Q_SIGNALS:
    void changedOutside();
    void deletedOutside();
    void pathChanged(const QString& path);
    void modifiedChanged(bool modified);

private Q_SLOTS:
    void onChange(const QString&);
    void onDirectoryChange(const QString& path);
    void onObjectNameChanged(const QString&);

private:
    void synchronizeObjectName();

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
