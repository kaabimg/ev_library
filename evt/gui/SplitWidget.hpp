#pragma once
#include <QSplitter>

namespace evt {
class Style;

class SplitWidget : public QSplitter {
    Q_OBJECT
public:
    SplitWidget(Qt::Orientation orientation, QWidget* parent = nullptr);

    void applyStyle(const Style&);

protected:
    QSplitterHandle* createHandle();
};
}
