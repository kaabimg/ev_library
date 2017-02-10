#pragma once

#include "../qtypes_fwd.hpp"

#include <qabstractitemmodel.h>
#include <qsortfilterproxymodel.h>

#include <functional>

namespace ev {
namespace atk {

class object_t;

struct object_data_proxy_t {
    using get_data_f = std::function<qvariant(object_t* object, int role)>;
    using get_header_data_f =
        std::function<qvariant(int section, Qt::Orientation orientation, int role)>;
    using set_data_f  = std::function<bool(object_t* object, int role, const qvariant& value)>;
    using get_flags_f = std::function<Qt::ItemFlags(object_t* object)>;

    ///////

    get_data_f get_data;
    get_header_data_f get_header_data;
    set_data_f set_data;
    get_flags_f get_flags;
    int column_count = -1;
};

class object_model_base_t : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit object_model_base_t(object_t*, qobject* parent = nullptr);
    object_t* root() const;

public:
    virtual qmodelindex index(int row, int column, const qmodelindex& parent = qmodelindex()) const;
    virtual qmodelindex parent(const qmodelindex& child) const;
    virtual int rowCount(const qmodelindex& parent = qmodelindex()) const;
    virtual int columnCount(const qmodelindex& parent = qmodelindex()) const;
    virtual QVariant data(const qmodelindex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const qmodelindex& index, const QVariant& value, int role);
    qvariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const qmodelindex& index) const;

Q_SIGNALS:
    void expand_request(const qmodelindex&);

protected Q_SLOTS:
    void on_hierarchy_changed();
    void on_data_changed(object_t*);

protected:
    object_t* object_for_index(const qmodelindex&) const;

private:
    object_t* m_root;
    object_data_proxy_t m_proxy;
    friend class object_model_t;
};

//////////////////////////////////////////////////////////////

struct filter_sort_proxy_t {
    using filter_f    = std::function<bool(object_t*)>;
    using less_than_f = std::function<bool(object_t*, object_t*)>;

    filter_f filter       = [](object_t*) { return true; };
    less_than_f less_than = [](object_t*, object_t*) { return true; };
};

class object_model_t : public QSortFilterProxyModel {
    Q_OBJECT
public:
    object_model_t(object_t*, qobject* parent);
    ~object_model_t();

    object_model_base_t* source() const;


    ///////////////////////////////////////////////

    inline void set_column_count(int count);

    template <typename T>
    inline void set_get_data_function(T&& f);

    template <typename T>
    inline void set_set_data_function(T&& f);

    template <typename T>
    inline void set_get_flags_function(T&& f);

    template <typename T>
    inline void set_get_header_data_function(T&& f);

    template <typename T>
    inline void set_filter(T&& f);

    template <typename T>
    inline void set_less_than(T&& f);

    inline void set_data_proxy(const object_data_proxy_t&);
    inline void set_filter_sort_proxy(const filter_sort_proxy_t&);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const;

private:
    filter_sort_proxy_t m_proxy;
};

template <typename T>
void object_model_t::set_get_data_function(T&& f)
{
    beginResetModel();
    source()->m_proxy.get_data = std::forward(f);
    endResetModel();
}

template <typename T>
void object_model_t::set_set_data_function(T&& f)
{
    beginResetModel();
    source()->m_proxy.set_data = std::forward(f);
    endResetModel();
}

template <typename T>
void object_model_t::set_get_flags_function(T&& f)
{
    beginResetModel();
    source()->m_proxy.get_flags = std::forward(f);
    endResetModel();
}

template <typename T>
void object_model_t::set_get_header_data_function(T&& f)
{
    beginResetModel();
    source()->m_proxy.get_header_data = std::forward(f);
    endResetModel();
}

template <typename T>
void object_model_t::set_filter(T&& f)
{
    beginResetModel();
    m_proxy.filter = std::forward(f);
    endResetModel();
}

template <typename T>
void object_model_t::set_less_than(T&& f)
{
    beginResetModel();
    m_proxy.less_than = std::forward(f);
    endResetModel();
}

}
}
