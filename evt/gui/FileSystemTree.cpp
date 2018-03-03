#include "FileSystemTree.hpp"
#include "Style.hpp"
#include "../style/stylesheets/TreeView.hpp"
#include <QFileSystemModel>

using namespace evt;

FileSystemTree::FileSystemTree(QWidget* parent) : QTreeView(parent)
{
    auto model = new QFileSystemModel(this);
    setModel(model);

    setDragEnabled(true);
    setHeaderHidden(true);
    setAnimated(true);

    hideColumn(1);
    hideColumn(2);
    hideColumn(3);

    connect(this, &FileSystemTree::doubleClicked, this, &FileSystemTree::onIndexDoubleClicked);

    monitorStyleChangeFor(this);
    applyStyle(appStyle());
}

QFileSystemModel* FileSystemTree::fsModel() const
{
    return reinterpret_cast<QFileSystemModel*>(model());
}

void FileSystemTree::openFolder(const QString& path)
{
    fsModel()->setRootPath(path);
    setRootIndex(fsModel()->index(path));
}

QString FileSystemTree::rootDir() const
{
    return fsModel()->rootPath();
}

void FileSystemTree::onIndexDoubleClicked(const QModelIndex& index)
{
    QString path = index.data(QFileSystemModel::FilePathRole).toString();

    if (QFileInfo(path).isFile()) {
        Q_EMIT openFileRequest(path);
    }
}

void FileSystemTree::applyStyle(const Style& s)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, s.theme.background.normal);
    setPalette(palette);
    setStyleSheet(s.adaptStyleSheet(treeViewStyleSheet));
}
