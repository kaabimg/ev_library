
include_directories(${BOOST_INCLUDE_DIR})

set (SOURCES
    common.cpp
    command_queue.cpp
    context.cpp
    device.cpp
    kernel.cpp
    platform.cpp
    program.cpp
)


set (HEADERS
    ocl.hpp
    common.hpp
    command_queue.hpp
    context.hpp
    device.hpp
    kernel.hpp
    platform.hpp
    program.hpp
)


FIND_PACKAGE(OpenCL REQUIRED)
LINK_DIRECTORIES(${OpenCL_LIBRARY})

add_library(ev_ocl SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ev_ocl OpenCL)
