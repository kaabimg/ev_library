cmake_minimum_required( VERSION 3.10 )

project(ev_library CXX)

find_package(Threads)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(ROOT_DIR   ${PROJECT_SOURCE_DIR}/)
SET(INSTALL_DIR  ${ROOT_DIR}/install)

SET(BIN_DIR    ${INSTALL_DIR}/bin)
SET(LIB_DIR    ${INSTALL_DIR}/lib)
SET(TEST_DIR   ${INSTALL_DIR}/test)

include_directories(${PROJECT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ${LIB_DIR})


#### Libraries

## boost
set(BOOSTROOT /media/ghaith/data/development/installs/boost/install)
find_package(Boost 1.63 REQUIRED system thread)
include_directories(${Boost_INCLUDE_DIRS})

## dlib
SET(DLIB_DIR          /media/ghaith/data/development/installs/dlib/install)
SET(DLIB_INCLUDE_DIR  ${DLIB_DIR}/include)
SET(DLIB_LIB_DIR      ${DLIB_DIR}/lib) #optionnal

##########



#########

add_subdirectory( ev )
add_subdirectory( evt )
add_subdirectory( bin )
add_subdirectory( tests )




