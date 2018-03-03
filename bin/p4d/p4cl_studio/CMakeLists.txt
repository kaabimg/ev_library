set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Qml)

file (GLOB STUDIO_FILES
    ./*.cpp
    ./*.hpp

    gui/*.cpp
    gui/*.hpp

    gui/editor/*.cpp
    gui/editor/*.hpp

    gui/graphview/*.cpp
    gui/graphview/*.hpp

    app/*.hpp
    app/*.cpp
    
    icons.qrc
    )


INCLUDE_DIRECTORIES(..)

add_executable (p4dcl_studio ${STUDIO_FILES})
target_link_libraries(p4dcl_studio Qt5::Core Qt5::Widgets Qt5::Qml p4d_cl evt)
