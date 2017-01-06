#include "type.hpp"
#include "private_data.hpp"

using namespace ev::vm;
using namespace ev::vm::jit;

std::string type_t::to_string() const {
    if (is_struct()) {
        struct_data_t s_data = (struct_data_t)m_data;
        return s_data->getName();
    } else {
        auto iter = builtin_type_names.find(kind());
        if (iter != builtin_type_names.end()) return iter->second;
    }

    assert(false && "Error in std::string type_t::to_string : unkwnown type");
    return "unknwon";
}

type_kind_e type_t::kind() const {
    switch (m_data->getTypeID()) {
        case Type::FloatTyID: return type_kind_e::r32;
        case Type::DoubleTyID: return type_kind_e::r64;
        case Type::IntegerTyID: return int_kind();
        case Type::StructTyID: return type_kind_e::structure;
        default: return type_kind_e::unknown;
    }
}

bool type_t::is_number() const {
    switch (m_data->getTypeID()) {
        case Type::FloatTyID:
        case Type::DoubleTyID:
        case Type::IntegerTyID: return true;
        default: return false;
    }
}

bool type_t::is_integer() const {
    return m_data->getTypeID() == Type::IntegerTyID;
}

bool type_t::is_floating_point() const {
    switch (m_data->getTypeID()) {
        case Type::FloatTyID:
        case Type::DoubleTyID: return true;
        default: return false;
    }
}

bool type_t::is_struct() const {
    return m_data->getTypeID() == Type::StructTyID;
}

type_kind_e type_t::int_kind() const {
    llvm::IntegerType* type = static_cast<llvm::IntegerType*>(m_data);
    switch (type->getBitWidth()) {
        case 32: return type_kind_e::i32;
        case 64: return type_kind_e::i64;
        default: return type_kind_e::unknown;
    }
}

struct_info_t& struct_info_t::operator<<(
    std::pair<type_t, std::string>&& field) {
    field_types.push_back(std::move(field.first));
    field_names.push_back(std::move(field.second));
    return *this;
}

const std::string& struct_t::name() const {
    return d->module->structs_data[d->data].name;
}

const std::vector<std::string>& struct_t::field_names() const {
    return d->module->structs_data[d->data].field_names;
}

const std::vector<type_t>& struct_t::field_types() const {
    return d->module->structs_data[d->data].field_types;
}

int struct_t::field_index(const std::string& name) const {
    int i = 0;
    for (auto& f : field_names()) {
        if (f == name) break;
        ++i;
    }
    if (i != field_names().size()) return i;
    assert(false &&
           "Error in int struct_t::field_index : unknown struct field");
    return -1;
}

type_t struct_t::to_type() const {
    return type_t(d->data);
}
