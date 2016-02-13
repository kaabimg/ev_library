#ifndef EV_VM_OBJECT_H
#define EV_VM_OBJECT_H

#include <memory>
#include <core/variant.h>

namespace ev { namespace vm {


struct vm_object_t {


    vm_object_t(){}

    template<typename T>
    vm_object_t(const T & v):m_value(v){}

    template<typename T>
    vm_object_t( T && v):m_value(v){}

    template<typename T>
    vm_object_t & operator =(const T & val){
        m_value = val;
        return *this;
    }

    template<typename T>
    vm_object_t & operator =( T && val){
        m_value = val;
        return *this;
    }


    core::variant_type_e type()const {
        return core::variant_type(m_value);
    }

    bool is_valid()const {
        return type() != core::variant_type_e::null;
    }

    const char* type_name()const {
        return core::variant_type_name(m_value);
    }

    template<typename T>
    T & as() {
        return core::variant_as<T>(m_value);
    }

    template<typename T>
    const T & as()const {
        return core::variant_as<T>(m_value);
    }

private:
    core::variant_t m_value;

};

}}


#endif // EV_VM_OBJECT_H
