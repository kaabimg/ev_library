#pragma once

#include "../../qtypes_fwd.hpp"
#include <qtablewidget.h>

namespace ev {
namespace atk {


class message_pane_t : public QTableWidget
{
    Q_OBJECT
public:
    message_pane_t(qwidget* parent = nullptr);
    ~message_pane_t();

public Q_SLOTS:
    void add_info(const qstring&);
    void add_warning(const qstring&);
    void add_error(const qstring&);
protected:
    void add_message(const qicon&,const qstring& msg);
};

}

}
