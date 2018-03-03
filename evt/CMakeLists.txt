set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Qml)

file (GLOB FRAMEWORK_FILES
    core/*.hpp
    core/*.cpp

    gui/*.hpp
    gui/*.cpp

    gui/textedit/*.hpp
    gui/textedit/*.cpp

    style/stylesheets/*.hpp
    ressources.qrc
    )

INCLUDE_DIRECTORIES(..)
add_library (evt SHARED ${FRAMEWORK_FILES} )
target_link_libraries(evt Qt5::Core Qt5::Widgets Qt5::Qml)
