#pragma once

#include <ev/core/pimpl.hpp>

#include <QWidget>


namespace evt {

class File;
class FoldedTextEdit;
class Style;

class FileEditor : public QWidget {
    Q_OBJECT
public:
    enum ContentState { Saved, Modified };
    Q_ENUMS(ContentState)

    explicit FileEditor(File* file, QWidget* parent = nullptr);
    ~FileEditor();
    void applyStyle(const Style&);

    void createContent();
    FoldedTextEdit* textEditor() const;

Q_SIGNALS:
    void removeFileRequest();

public Q_SLOTS:
    void save(bool force = false);
    void forceSave();
    void saveAs();
    void read();
    void highlightText(const QString&, bool outlineOnly = false);
    void findNext(const QString&);
    void findPrevious(const QString&);

protected Q_SLOTS:
    void onFindRequest();
    void onSelectionChanged();

protected:
    void closeEvent(QCloseEvent *event);

    virtual FoldedTextEdit* createEditor(File*);

private:
    class Impl;
    ev::pimpl<Impl> d;
};
}
