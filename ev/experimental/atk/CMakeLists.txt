include_directories(${BOOST_INCLUDE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


file (GLOB CORE_HEADERS
    *.hpp
    ./core/*.hpp
    )

file (GLOB GUI_HEADERS
    ./gui/*.hpp
    ./gui/main_window/*.hpp
    ./gui/widgets/*.hpp

    )

file (GLOB CORE_SOURCES
    ./core/*.cpp
    )
file (GLOB GUI_SOURCES
    ./gui/*.cpp
    ./gui/main_window/*.cpp
    ./gui/widgets/*.cpp
    )

file (GLOB RESSOURCES
    ./resources/*.qrc)

add_library(ev_atk SHARED ${CORE_HEADERS} ${GUI_HEADERS} ${GUI_SOURCES} ${CORE_SOURCES} ${RESSOURCES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core)
find_package(Qt5Widgets)
qt5_use_modules(ev_atk Core Widgets)
target_link_libraries(ev_atk ${QT_LIBRARIES})

