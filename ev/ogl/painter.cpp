#include "painter.hpp"
#include "window.hpp"
#include "shader.hpp"

#include "glfw.hpp"

using namespace ev::ogl;

painter_t::painter_t(window_t& w)
{
    w.make_current();
}

void painter_t::finish()
{
    EV_OGL_CALL(glFinish());
}

void painter_t::set_viewport(position_t pos, size_2d_t size)
{
    EV_OGL_CALL(glViewport(pos.x, pos.y, size.width, size.height));
}

void painter_t::clear(GLbitfield mask)
{
    glClear(mask);
}

void painter_t::use(const program_t& program)
{
    EV_OGL_CALL(glUseProgram(program.gl_object()));
}

void painter_t::set_line_width(float w)
{
    EV_OGL_CALL(glLineWidth(w));
}

void painter_t::enable(GLenum)
{
    EV_OGL_CALL(glEnable(GL_BLEND));
}

void painter_t::disable(GLenum)
{
    EV_OGL_CALL(glDisable(GL_BLEND));
}
