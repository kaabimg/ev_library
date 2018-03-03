set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file (GLOB SRC_FILES
	*.cpp
	*.hpp

	parser/*.cpp
	parser/*.hpp

    compiler/*.cpp
    compiler/*.hpp

    analysis/*.cpp
    analysis/*.hpp
)

find_package(Qt5Core)
find_package(Qt5Qml)

INCLUDE_DIRECTORIES(..)
add_library(p4d_cl SHARED ${SRC_FILES})
target_link_libraries(p4d_cl Qt5::Core  Qt5::Qml)
