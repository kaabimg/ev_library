include_directories(${BOOST_INCLUDE_DIR})

set (SOURCES
    context.cpp
    shader.cpp
    )

set (HEADERS
    common.hpp
    context.hpp
    shader.hpp
    ogl.hpp
    )

FIND_PACKAGE(OpenGL 2.0 REQUIRED)
LINK_DIRECTORIES(${OPENGL_INCLUDE_DIR})

add_library(ev_ogl SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_ogl GLESv2)

