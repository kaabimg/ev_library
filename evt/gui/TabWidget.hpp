#pragma once

#include <QTabWidget>

namespace evt {
class Style;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    TabWidget(QWidget* parent = nullptr);
    void applyStyle(const Style&);
};

}
