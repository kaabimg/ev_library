include_directories(${BOOST_INCLUDE_DIR})

set (SOURCES
    common.cpp
    engine.cpp
    )

set (HEADERS
    common.hpp
    engine.hpp
    vlr.hpp
    )

FIND_PACKAGE(OpenGL 2.0 REQUIRED)
LINK_DIRECTORIES(${OPENGL_INCLUDE_DIR})

add_library(ev_vlr SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_vlr ev_ogl GLESv2 glfw)

