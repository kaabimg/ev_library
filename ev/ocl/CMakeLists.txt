
include_directories(${BOOST_INCLUDE_DIR})

set (SOURCES
    common.cpp
    command_queue.cpp
    context.cpp
    device.cpp
    kernel.cpp
    platform.cpp
    program.cpp
    memory_object.cpp
    action.cpp
    event.cpp
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
    memory_object.hpp
    action.hpp
    nd_range.hpp
    event.hpp
    debug.hpp
)


FIND_PACKAGE(OpenCL)
if(OpenCL_FOUND)
    LINK_DIRECTORIES(${OpenCL_LIBRARY})
    add_library(ev_ocl SHARED ${SOURCES} ${HEADERS})
    target_link_libraries (ev_ocl OpenCL)
endif()

