#include <ev/experimental/ogl/debug.hpp>
#include <ev/core/logging.hpp>

#define EV_OGL
#ifdef EV_OGL
using namespace ev::ogl;

inline std::vector<vec2_t<float>> generate_points(auto&& f)
{
    std::vector<vec2_t<float>> points;

    for (float x = -10.0; x <= 10.0; x += 0.1) {
        points.push_back({x / 10, (float)f(x)});
    }
    return points;
}


template <typename A, typename B, typename C>
struct foo_t {
    A a;
    B b;
    C c;

    foo_t(A _a, B _b, C _c) : a(_a), b(_b), c(_c)
    {
    }
};

int main(int argc, char* argv[])
{
    try {
        application_t app{argc, argv};


        app.set_property("name", "evgl");
        app.set_property("major_v", 0);
        app.set_property("minor_v", 1);

        ev::debug() << "Application :" << app.property("name").as<const char*>()
                    << app.property("major_v").as<int>()
                    << app.property("minor_v").as<int>();

        window_t window{{600, 400}, argv[0]};

        painter_t painter{window};

        program_t program;

        program.create_shader_from_file(shader_type_e::vertex,
                                        "/home/ghaith/data/development/"
                                        "workspace/ev_library/data/ev/ogl/"
                                        "vertex.vsh");
        program.create_shader_from_file(
            shader_type_e::fragment,
            "/home/ghaith/data/development/workspace/"
            "ev_library/data/ev/ogl/fragment.fsh");

        program.link();

        ev::debug() << "Attributes" << program.get_attribute_names();
        ev::debug() << "Uniforms  " << program.get_uniform_names();

        ev::debug() << program.get_attribute_location("position");
        program.bind_attribute(0, "pos");

        std::vector<vec2_t<float>> points = generate_points(sin);

        buffer_t<vec2_t<float>, GL_ARRAY_BUFFER> buffer;
        ev::debug() << "buffer id" << buffer.gl_object();
        buffer.set_data(generate_points(sin));
        buffer.upload();
        ev::debug() << "buffer id" << buffer.gl_object();

        window.set_paint_func([&](window_t& w) {

            painter.set_viewport({0, 0}, w.size());
            painter.clear(GL_COLOR_BUFFER_BIT);
            painter.use(program);

            painter.set_line_width(1.f);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Load the vertex data

            EV_OGL_CALL(glEnableVertexAttribArray(
                program.get_attribute_location("position")));

            EV_OGL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0,
                                              points.data()));

            EV_OGL_CALL(glDrawArrays(GL_LINE_STRIP, 0, points.size()));

        });

        window.show();
        return app.exec();
    }
    catch (const gl_error_t& exception) {
        ev::error() << exception.error() << exception.what();
        return EXIT_FAILURE;
    }
    catch (const std::exception& exception) {
        ev::error() << exception.what();
        return EXIT_FAILURE;
    }
}
#else

#include "linmath.h"
#include <ev/ogl/glfw.hpp>

#include <stdlib.h>
#include <stdio.h>

const struct {
    float x, y;
    float r, g, b;
} vertices[3] = {{-0.6f, -0.4f, 1.f, 0.f, 0.f},
                 {0.6f, -0.4f, 0.f, 1.f, 0.f},
                 {0.f, 0.6f, 0.f, 0.f, 1.f}};

static const char* vertex_shader_text =
    "#version 110\n"
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";

static const char* fragment_shader_text =
    "#version 110\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    //    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location  = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

#endif
