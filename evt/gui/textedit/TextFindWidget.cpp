#include "TextFindWidget.hpp"

#include "../Style.hpp"
#include "../../style/stylesheets/Button.hpp"
#include "../../style/stylesheets/LineEdit.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QKeyEvent>
#include <QPushButton>

using namespace evt;

TextFindWidget::TextFindWidget(QWidget* parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    _mainLayout = new QVBoxLayout(this);
    _findLayout = new QHBoxLayout();
    _replaceLayout = new QHBoxLayout();

    _mainLayout->addLayout(_findLayout);
    _mainLayout->addLayout(_replaceLayout);

    static const int labelWidth = 100;
    static const int lineEditWidth = 250;
    static const int nextPreviousWidth = 50;

    _findLabel = new QLabel("Find:", this);
    _findLabel->setFixedWidth(labelWidth);

    _findLineEdit = new QLineEdit(this);
    _findLineEdit->setFixedWidth(lineEditWidth);

    _findNextButton = createButton(">>", this);
    _findPreviousButton = createButton("<<", this);
    _findNextButton->setFixedWidth(nextPreviousWidth);
    _findPreviousButton->setFixedWidth(nextPreviousWidth);

    _findLayout->addWidget(_findLabel);
    _findLayout->addWidget(_findLineEdit);
    _findLayout->addWidget(_findPreviousButton);
    _findLayout->addWidget(_findNextButton);
    _findLayout->addWidget(new QWidget(this));  // spacer

    _replaceLabel = new QLabel("Replace with:", this);
    _replaceLabel->setFixedWidth(labelWidth);

    _replaceLineEdit = new QLineEdit(this);
    _replaceLineEdit->setFixedWidth(lineEditWidth);

    _replaceButton = createButton("Replace", this);
    _replaceAndFind = createButton("Replace & Find", this);
    _replaceAllButton = createButton("Replace All", this);

    _replaceLayout->addWidget(_replaceLabel);
    _replaceLayout->addWidget(_replaceLineEdit);
    _replaceLayout->addWidget(_replaceButton);
    _replaceLayout->addWidget(_replaceAndFind);
    _replaceLayout->addWidget(_replaceAllButton);
    _replaceLayout->addWidget(new QWidget(this));  // spacer

    connect(_findLineEdit, &QLineEdit::textChanged, this, &TextFindWidget::findTextChanged);
    connect(_findNextButton, &QPushButton::clicked, this, &TextFindWidget::onFindNextClicked);
    connect(_findPreviousButton, &QPushButton::clicked, this,
            &TextFindWidget::onFindPreviousClicked);
}

void TextFindWidget::applyStyle(const Style& s)
{
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Background, s.theme.textEdit.backgroundColor);
    p.setColor(QPalette::WindowText, s.theme.textEdit.textColor);
    setPalette(p);
    _findLabel->setPalette(p);
    _replaceLabel->setPalette(p);

    QString bss = s.adaptStyleSheet(toolbarButtonStyleSheet);
    _findNextButton->setStyleSheet(bss);
    _findPreviousButton->setStyleSheet(bss);
    _replaceButton->setStyleSheet(bss);
    _replaceAndFind->setStyleSheet(bss);
    _replaceAllButton->setStyleSheet(bss);

    _findLineEdit->setStyleSheet(s.adaptStyleSheet(lineEditStyleSheet));
    _replaceLineEdit->setStyleSheet(s.adaptStyleSheet(lineEditStyleSheet));
}
void TextFindWidget::fillAndShow(const QString& findText)
{
    show();
    if (findText == _findLineEdit->text()) {
        Q_EMIT findTextChanged(findText);
    }
    else {  // signal will be emitted when setting the text
        _findLineEdit->setText(findText);
    }
    setFocus();
}

void TextFindWidget::onFindNextClicked()
{
    Q_EMIT findNextRequest(_findLineEdit->text());
}

void TextFindWidget::onFindPreviousClicked()
{
    Q_EMIT findPreviousRequest(_findLineEdit->text());
}

void TextFindWidget::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        Q_EMIT finished();
        ;
        hide();
    }
    else {
        QWidget::keyPressEvent(e);
    }
}

QPushButton* TextFindWidget::createButton(const QString& text, QWidget* parent) const
{
    QPushButton* button = new QPushButton(parent);
    button->setText(text);
    return button;
}
