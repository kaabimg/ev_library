#include "TextEditStatusWidget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QTimer>

using namespace evt;

TextEditStatusWidget::TextEditStatusWidget(QWidget* parent) : QWidget(parent)
{
    setAutoFillBackground(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(0);
    _positionLabel = new QLabel(this);
    _gotoLineLabel = new QLabel(this);
    _gotoLineLineEdit = new QLineEdit(this);

    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _gotoLineLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    _gotoLineLineEdit->setMaximumWidth(60);
    _gotoLineLineEdit->setAlignment(Qt::AlignHCenter);

    _gotoLineLabel->setText("goto ");

    layout->addWidget(spacer);
    layout->addWidget(_gotoLineLabel);
    layout->addWidget(_gotoLineLineEdit);
    layout->addWidget(_positionLabel);

    setCursorPosition(0, 0);

    connect(_gotoLineLineEdit, &QLineEdit::textEdited, &_setCurorPositionTimer,
            qOverload<>(&QTimer::start));

    _setCurorPositionTimer.setInterval(1000);
    _setCurorPositionTimer.setSingleShot(true);
    connect(&_setCurorPositionTimer, &QTimer::timeout, this,
            &TextEditStatusWidget::onCursorPositionEdited);
}

void TextEditStatusWidget::setBackgroundColor(const QColor& color)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, color);
    setPalette(palette);
}

void TextEditStatusWidget::setTextColor(const QColor& color)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Foreground, color);
    setPalette(palette);
    _positionLabel->setPalette(palette);
    _gotoLineLabel->setPalette(palette);
}

void TextEditStatusWidget::setCursorPosition(uint line, uint column)
{
    _positionLabel->setText(" Line: " + QString::number(line) +
                            ", Col: " + QString::number(column) + " ");
}

void TextEditStatusWidget::setFont(const QFont& font)
{
    QFont f = font;
    //    f.setBold(true);
    _positionLabel->setFont(f);
    _gotoLineLabel->setFont(f);
    _gotoLineLineEdit->setFont(f);
}

void TextEditStatusWidget::setFileModified(bool modified)
{
    _fileModified = modified;
}

void TextEditStatusWidget::onCursorPositionEdited()
{
    bool ok;
    uint line = _gotoLineLineEdit->text().toUInt(&ok);
    if (!ok) {
        return;
    }

    if (line) {
        Q_EMIT setCursorPositionRequest(line, 1);
    }
}
