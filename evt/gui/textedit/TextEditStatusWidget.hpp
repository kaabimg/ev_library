#pragma once

#include <QWidget>
#include <QTimer>

class QLabel;
class QLineEdit;

namespace evt {

class TextEditStatusWidget : public QWidget {
    Q_OBJECT
public:
    explicit TextEditStatusWidget(QWidget* parent = 0);    

Q_SIGNALS:
    void setCursorPositionRequest(uint, uint);
public Q_SLOTS:
    void setBackgroundColor(const QColor&);
    void setTextColor(const QColor&);
    void setCursorPosition(uint line, uint column);
    void setFont(const QFont& font);
    void setFileModified(bool);

protected Q_SLOTS:
    void onCursorPositionEdited();

private:
    QString _fileName;
    QTimer _setCurorPositionTimer;
    QLabel* _positionLabel;
    QLabel* _gotoLineLabel;
    QLineEdit* _gotoLineLineEdit;
    bool _fileModified = false;
};
}
