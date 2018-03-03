#pragma once

#include <QWidget>

class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QLabel;

namespace evt {

class Style;

class TextFindWidget : public QWidget {
    Q_OBJECT
public:
    explicit TextFindWidget(QWidget* parent = 0);

    void applyStyle(const Style&);

Q_SIGNALS:
    void findTextChanged(const QString&);
    void findNextRequest(const QString&);
    void findPreviousRequest(const QString&);
    void replaceOneRequest(const QString& text, const QString& replacement);
    void replaceAllRequest(const QString& text, const QString& replacement);

    void finished();
public Q_SLOTS:
    void fillAndShow(const QString& findText);

protected Q_SLOTS:
    void onFindNextClicked();
    void onFindPreviousClicked();

protected:
    void keyPressEvent(QKeyEvent*);
    QPushButton* createButton(const QString& text, QWidget* parent) const;

private:
    QVBoxLayout* _mainLayout;
    QHBoxLayout *_findLayout, *_replaceLayout;
    QLineEdit *_findLineEdit, *_replaceLineEdit;
    QLabel *_findLabel, *_replaceLabel;
    QPushButton *_findNextButton, *_findPreviousButton;
    QPushButton *_replaceButton, *_replaceAndFind, *_replaceAllButton;
};
}
