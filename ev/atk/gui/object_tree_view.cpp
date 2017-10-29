#include "object_tree_view.hpp"
#include "object_model.hpp"
using namespace ev::atk;

struct object_tree_view_t::impl {
    object_model_t* model;
};

object_tree_view_t::object_tree_view_t(object_t* root, qwidget* parent)
    : QTreeView(parent), d(new impl)
{
    d->model = new object_model_t(root,this);
    setModel(d->model);
    setHeaderHidden(true);
}
object_tree_view_t::~object_tree_view_t()
{
    delete d;
}
