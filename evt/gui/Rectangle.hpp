#pragma once
#include <QWidget>

namespace evt {


class Rectangle : public QWidget
{
    Q_OBJECT
public:
    Rectangle(QWidget* parent = nullptr);
    void setColor(const QColor&);

};

}
