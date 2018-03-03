#pragma once

#include <QWidget>

class QHBoxLayout;
class QLabel;
class QPushButton;

namespace evt {
class Style;

class FileChangedWidget : public QWidget {
    Q_OBJECT
public:
    explicit FileChangedWidget(QWidget* parent = 0);

    void applyStyle(const Style&);

Q_SIGNALS:
    void reloadRequest();
    void closeRequest();
    void saveRequest();

public Q_SLOTS:
    void askForFileChangedOutside();
    void askForFileDeletedOutside();

private:
    QHBoxLayout* _layout;
    QLabel* _text;
    QPushButton *_close, *_reload, *_cancel, *_save;
};
}
