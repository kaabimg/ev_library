#pragma once

#include "../qtypes_fwd.hpp"
#include <ev/core/preprocessor.hpp>
#include <qtreeview.h>

namespace ev {
namespace atk {

class object_t;

class object_tree_view_t : public QTreeView {
    Q_OBJECT
public:
    object_tree_view_t(object_t* root, qwidget* parent = nullptr);
    ~object_tree_view_t();

private:
    EV_IMPL(object_tree_view_t)

};

}
}
