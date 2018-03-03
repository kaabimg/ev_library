#include "File.hpp"

#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

using namespace evt;

struct File::Impl {
    QString path;
    QString error;
    QDateTime lastModified;
    QFileSystemWatcher* watcher;
    bool isWatched, modified = false;
};

File::File(const QString& path, QObject* parent) : QObject(parent)
{
    d->path = path;
    d->watcher = new QFileSystemWatcher(this);
    d->isWatched = false;
    QFileInfo fi(path);
    if (fi.exists()) {
        d->lastModified = fi.lastModified();
    }
    synchronizeObjectName();
    watch();
}

File::~File()
{
    unwatch();
}

QString File::fileName() const
{
    return QFileInfo(d->path).fileName();
}
QString File::path() const
{
    return d->path;
}

void File::setPath(const QString& path)
{
    QFileInfo fi(path);

    if (!d->path.isEmpty() && QFileInfo(d->path) == fi) {
        return;
    }
    unwatch();
    d->path = path;
    synchronizeObjectName();
    if (exists()) {
        d->lastModified = fi.lastModified();
    }
    watch();
    Q_EMIT pathChanged(path);
}

bool File::isValid() const
{
    return !path().isEmpty();
}

QString File::dir() const
{
    return QFileInfo(path()).absoluteDir().absolutePath();
}
QString File::errorString() const
{
    return d->error;
}

bool File::modified() const
{
    return d->modified;
}

void File::watch()
{
    if (exists() && !d->isWatched) {
        d->watcher->addPath(path());

        connect(d->watcher, &QFileSystemWatcher::fileChanged, this, &File::onChange);
        d->isWatched = true;
    }
    else {
        d->isWatched = false;
    }
}

void File::unwatch()
{
    if (d->isWatched) {
        d->watcher->disconnect();
        d->watcher->removePath(path());
        //        _impl->watcher->removePath(dir());
        d->isWatched = false;
    }
}

void File::rewatch()
{
    unwatch();
    watch();
}

void File::setModified(bool modified)
{
    if (d->modified == modified) return;

    d->modified = modified;
    Q_EMIT modifiedChanged(modified);
}

void File::synchronizeObjectName()
{
    if (exists()) {
        disconnect(this, &File::objectNameChanged, this, &File::onObjectNameChanged);
        setObjectName(fileName());
        connect(this, &File::objectNameChanged, this, &File::onObjectNameChanged);
    }
}

void File::onChange(const QString& path)
{
    QFileInfo changedPath(path);
    QFileInfo fi(d->path);
    if (fi == changedPath && d->lastModified != fi.lastModified()) {
        if (changedPath.exists()) {
            d->lastModified = fi.lastModified();
            Q_EMIT changedOutside();
        }
        else {
            Q_EMIT deletedOutside();
        }
    }
}

void File::onDirectoryChange(const QString& path)
{
    if (QFileInfo(dir()) == QFileInfo(path)) {
        if (!exists()) {
            Q_EMIT deletedOutside();
        }
    }
    Q_UNUSED(path);
}

void File::onObjectNameChanged(const QString& name)
{
    if (fileName() != name) {
        rename(name);
    }
}

bool File::exists() const
{
    return QFile::exists(path());
}

bool File::rename(const QString& name)
{
    QFile f(d->path);
    if (f.exists()) {
        QString dir = this->dir();
        unwatch();
        bool ok = f.rename(name);
        if (ok) {
            synchronizeObjectName();
            setPath(dir + "/" + name);
            setModified(false);
        }
        else {
            watch();
        }
        return ok;
    }
    return false;
}
bool File::save(const QString& data)
{
    if (save(data, path())) {
        d->lastModified = QFileInfo(d->path).lastModified();
        synchronizeObjectName();
        setModified(false);
        return true;
    }
    d->error = QString("Failed to open %0 in write mode").arg(path());
    return false;
}

bool File::save(const QString& data, const QString& path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data.toLatin1());
        file.flush();
        file.close();
        return true;
    }
    return false;
}

QDateTime File::lastModified() const
{
    return d->lastModified;
}

QString File::readContent(bool* openOk)
{
    if (openOk) {
        (*openOk) = false;
    }
    if (!exists()) {
        d->error = QString("Failed to open %0: File doesn't exist").arg(path());
        return "";
    }
    QFile me(path());
    if (me.open(QIODevice::ReadOnly)) {
        QString data = me.readAll();
        me.close();

        if (openOk) {
            (*openOk) = true;
        }
        return data;
    }
    d->error = QString("Failed to open %0 in read mode").arg(path());
    return "";
}
