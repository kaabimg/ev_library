#include "FileChangedWidget.hpp"
#include "../Style.hpp"
#include "../../style/stylesheets/Button.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

using namespace evt;

FileChangedWidget::FileChangedWidget(QWidget* parent) : QWidget(parent)
{
    _layout = new QHBoxLayout(this);
    _layout->setMargin(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    _text = new QLabel(this);
    _text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _close = new QPushButton(this);
    _close->setText("Close");

    _reload = new QPushButton(this);
    _reload->setText("Reload");

    _cancel = new QPushButton(this);
    _cancel->setText("Cancel");

    _save = new QPushButton(this);
    _save->setText("Save");

    connect(_close, &QPushButton::clicked, this, &FileChangedWidget::hide);
    connect(_reload, &QPushButton::clicked, this, &FileChangedWidget::hide);
    connect(_cancel, &QPushButton::clicked, this, &FileChangedWidget::hide);
    connect(_save, &QPushButton::clicked, this, &FileChangedWidget::hide);

    connect(_close, &QPushButton::clicked, this, &FileChangedWidget::closeRequest);
    connect(_reload, &QPushButton::clicked, this, &FileChangedWidget::reloadRequest);
    connect(_save, &QPushButton::clicked, this, &FileChangedWidget::saveRequest);

    _layout->addWidget(_text);
    _layout->addWidget(_reload);
    _layout->addWidget(_cancel);
    _layout->addWidget(_save);
    _layout->addWidget(_close);
}

void FileChangedWidget::applyStyle(const Style& s)
{
    setAutoFillBackground(true);

    QPalette p = _text->palette();
    p.setColor(QPalette::WindowText, s.theme.background.normal);
    _text->setPalette(p);

    p = palette();
    p.setColor(QPalette::Background, s.theme.background.normal);
    setPalette(p);

    QString bss = s.adaptStyleSheet(toolbarButtonStyleSheet);
    _reload->setStyleSheet(bss);
    _cancel->setStyleSheet(bss);
    _save->setStyleSheet(bss);
    _close->setStyleSheet(bss);
}

void FileChangedWidget::askForFileChangedOutside()
{
    _text->setText("  File modified outside editor");
    _reload->show();
    _cancel->show();
    _save->hide();
    _close->hide();
    setVisible(true);
}

void FileChangedWidget::askForFileDeletedOutside()
{
    _text->setText("  File deleted");
    _reload->hide();
    _cancel->hide();
    _save->show();
    _close->show();
    setVisible(true);
}
