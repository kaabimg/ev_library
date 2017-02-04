#pragma once
#include "../../qtypes_fwd.hpp"
#include <qtoolbar.h>

class QLabel;

namespace ev {
namespace atk {
class tool_bar_t : public QToolBar {
    Q_OBJECT
public:
    explicit tool_bar_t(qwidget* parent = nullptr);

    void set_label(const qstring& );
    void set_actions(const qlist<qaction*>& actions);


protected:
    void paintEvent(QPaintEvent*);

private:
    QLabel* m_label;
};
}
}
