#pragma once

#include "function_signature.hpp"
#include "data_fwd_declare.hpp"
#include "object.hpp"

namespace ev {
namespace vm {
namespace jit {
struct type {
    type(type_data data) : m_data(data)
    {
    }
    type()
    {
    }
    std::string to_string() const;

    type_kind kind() const;
    type_data data() const
    {
        return m_data;
    }
    operator type_data() const
    {
        return m_data;
    }
    operator bool() const
    {
        return m_data;
    }
    // utils
    bool is_number() const;
    bool is_integer() const;
    bool is_floating_point() const;
    bool is_struct() const;

protected:
    type_kind int_kind() const;

protected:
    type_data m_data = nullptr;
    friend class context;
};

struct structure_info {
    std::string name;
    std::vector<type> field_types;
    std::vector<std::string> field_names;
    //

    structure_info(const std::string& n) : name(n)
    {
    }
    structure_info(std::string&& n) : name(std::move(n))
    {
    }
    structure_info()
    {
    }
    structure_info& operator<<(std::pair<type, std::string>&&);
};

class structure_private;

struct structure : object<structure_private> {
    const std::string& name() const;
    const std::vector<std::string>& field_names() const;
    const std::vector<type>& field_types() const;

    int field_index(const std::string& name) const;

    type to_type() const;

    friend class module;
};
}
}
}
