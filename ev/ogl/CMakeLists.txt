include_directories(${BOOST_INCLUDE_DIR})

set (SOURCES
    common.cpp
    shader.cpp
    window.cpp
    application.cpp
    painter.cpp
    geometry.cpp
    )

set (HEADERS
    glfw.hpp
    common.hpp
    shader.hpp
    window.hpp
    application.hpp
    events.hpp
    painter.hpp
    lin_math.hpp
    buffer.hpp
    geometry.hpp
    debug.hpp
    ogl.hpp
    )

FIND_PACKAGE(OpenGL 2.0 REQUIRED)
LINK_DIRECTORIES(${OPENGL_INCLUDE_DIR})

add_library(ev_ogl SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_ogl GLESv2 glfw)

