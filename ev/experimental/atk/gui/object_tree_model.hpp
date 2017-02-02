#pragma once

#include "../qtypes_fwd.hpp"

#include <qabstractitemmodel.h>

#include <functional>

namespace ev {
namespace atk {

class object_t;

class object_tree_model_base_t : public QAbstractItemModel {
    Q_OBJECT
public:
    using data_getter_f = std::function<qvariant(object_t*, int)>;
    using data_setter_f = std::function<void(object_t*, int,const qvariant&)>;

    enum DataRole { IsClosableRole = Qt::UserRole + 1 };

    explicit object_tree_model_base_t(object_t*, qobject* parent = 0);
    object_t* root() const;

public:
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;

Q_SIGNALS:
    void expandRequest(const QModelIndex&);

protected Q_SLOTS:
    void on_hierarchy_changed();
    void on_data_changed(object_t*);

protected:
    object_t* objectForIndex(const QModelIndex&) const;

private:
    object_t* m_root;
    data_getter_f m_data_getter = nullptr;
};
}
}
