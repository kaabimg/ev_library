#pragma once

#include <qtoolbar.h>

class QLabel;

namespace ev {
namespace atk {
class tool_bar_t : public QToolBar {
    Q_OBJECT
public:
    explicit tool_bar_t(int label_width, QWidget* parent = nullptr);

public Q_SLOTS:
    void set_current_widget(QWidget*);

Q_SIGNALS:
    void current_widget_changed(QWidget*);

protected:
    void set_actions(const QList<QAction*>& actions);
    void paintEvent(QPaintEvent*);
protected Q_SLOTS:
    void reset();

private:
    QLabel* m_label;
    QWidget* m_current_widget;
};
}
}
