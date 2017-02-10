#pragma once
#include "common.hpp"
#include "lin_math.hpp"

namespace ev
{
namespace ogl
{
class window_t;
class program_t;
class painter_t
{
public:
    painter_t(window_t& w);

    void finish();
    void set_viewport(position_t pos, size_2d_t size);
    void clear(GLbitfield mask);
    void use(const program_t& program);

    void set_line_width(float);

    void enable(GLenum);
    void disable(GLenum);
};
}
}
