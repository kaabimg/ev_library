#include "Rectangle.hpp"

evt::Rectangle::Rectangle(QWidget* parent) : QWidget(parent)
{
}

void evt::Rectangle::setColor(const QColor& color)
{
    QPalette p(this->palette());
    p.setColor(QPalette::Background, color);
    setAutoFillBackground(true);
    setPalette(p);
}
