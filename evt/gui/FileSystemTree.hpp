#pragma once

#include <QTreeView>
class QFileSystemModel;

namespace evt {

class Style;

class FileSystemTree : public QTreeView {
    Q_OBJECT
public:
    explicit FileSystemTree(QWidget* parent = 0);

    QFileSystemModel* fsModel() const;

    void openFolder(const QString&);
    QString rootDir() const;

    void applyStyle(const Style&);

Q_SIGNALS:
    void openFileRequest(const QString&);

protected Q_SLOTS:
    void onIndexDoubleClicked(const QModelIndex&);

};
}
