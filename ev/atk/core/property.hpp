#pragma once

#include "../qtypes_fwd.hpp"
#include <ev/core/preprocessor.hpp>
#include <qobject.h>

namespace ev {
namespace atk {

enum class property_data_type_e { integer, real, string, file, directory, color };

enum class property_attribute_e { label, enabled, visible };

enum property_type_e { object_property, standalone };

class property_t : public qobject {
    Q_OBJECT
public:
    property_t(property_data_type_e type, qobject* obj, const qbytearray& name);
    property_t(property_data_type_e type, const qvariant& value);
    ~property_t();

    property_type_e type() const;
    property_data_type_e data_type() const;

    qvariant read() const;
    void write(const qvariant&);

    qvariant attribute(property_attribute_e a) const;
    void set_attribute(property_attribute_e a, const qvariant& val);

Q_SIGNALS:
    void value_changed();
    void attribute_changed(property_attribute_e);

private:
    EV_IMPL(property_t)
};

class property_group_t : public qobject {
    Q_OBJECT
public:
    property_group_t(qobject* parent = nullptr);
    ~property_group_t();
    void add_property(property_t*);
    void remove_property(property_t*);

    void add_sbugroup(property_group_t*);
    void remove_sbugroup(property_group_t*);

    const qvector<property_t*>& properties() const;
    const qvector<property_group_t*> subgroups() const;

    qvariant attribute(property_attribute_e a) const;
    void set_attribute(property_attribute_e a, const qvariant& val);

Q_SIGNALS:
    void attribute_changed(property_attribute_e);

private:
    EV_IMPL(property_group_t)
};
}
}
