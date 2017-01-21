#pragma once

#include "common.hpp"
#include "glfw.hpp"

#include <vector>
#include <tuple>

namespace ev {
namespace ogl {

enum class upload_policy_e { keep_host_data, clear_host_data };

inline void clearBuffer(GLuint id) {
    if (id) glDeleteBuffers(1, &id);
}

template <typename Data, unsigned int Type>
class buffer_t : public gl_object_wrapper_t<GLuint, clearBuffer> {
public:
    static_assert(Type == GL_ARRAY_BUFFER || Type == GL_ELEMENT_ARRAY_BUFFER,
                  "Invalid buffer type");

    static constexpr size_t element_size = sizeof(Data);

    buffer_t() {}
    ~buffer_t() { clear(); }
    buffer_t(std::initializer_list<Data> list) : m_data{list} {}
    buffer_t(buffer_t&& another) {
        m_data = std::move(another.m_data);
        std::swap(m_id, another.m_id);
    }

    void set_data(const std::vector<Data> & data)
    {
        m_data = data;
    }
    void set_data(std::vector<Data> && data)
    {
        m_data = std::move(data);
    }


    void reserve(size_t count) { m_data.reserve(count); }
    template <typename T>
    void push_back(T&& a) {
        m_data.push_back(std::forward(a));
    }

    void upload() {
        EV_OGL_CALL(glGenBuffers(1, &m_id));
        if (!m_id) throw gl_error_t("Failed to create buffer");
        EV_OGL_CALL(glBindBuffer(Type, m_id));
        EV_OGL_CALL(glBufferData(Type, m_data.size() * element_size, this,
                                 GL_STATIC_DRAW));
    }

    void clear_host_data() { m_data.clear(); }
    void clear() {
        clearBuffer(m_id);
        m_id = 0;
    }

private:
    std::vector<Data> m_data;
};
}
}
