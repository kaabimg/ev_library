include_directories(${BOOST_INCLUDE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


file (GLOB HEADERS
    *.hpp
    ./interfaces/*.hpp
    ./core/*.hpp
    ./gui/*.hpp
    ./gui/main_window/*.hpp
    ./gui/widgets/*.hpp

    )



file (GLOB SOURCES
    ./core/*.cpp
    ./gui/*.cpp
    ./gui/main_window/*.cpp
    ./gui/widgets/*.cpp
    )


file (GLOB RESSOURCES
    ./resources/*.qrc)

add_library(ev_atk SHARED ${HEADERS} ${HEADERS} ${SOURCES} ${RESSOURCES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core)
find_package(Qt5Widgets)
qt5_use_modules(ev_atk Core Widgets)
target_link_libraries(ev_atk ${QT_LIBRARIES})

