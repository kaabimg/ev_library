cmake_minimum_required( VERSION 3.4 )

project(ev_library CXX)

find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

SET(ROOT_DIR   ${PROJECT_SOURCE_DIR}/)
SET(INSTALL_DIR  ${ROOT_DIR}/install)

SET(BIN_DIR    ${INSTALL_DIR}/bin)
SET(LIB_DIR    ${INSTALL_DIR}/lib)
SET(TEST_DIR   ${INSTALL_DIR}/test)

include_directories(${PROJECT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ${LIB_DIR})


#### Libraries

## boost

SET(BOOST_DIR         /media/ghaith/data/development/installs/boost/install)
SET(BOOST_INCLUDE_DIR ${BOOST_DIR}/include)
SET(BOOST_LIB_DIR     ${BOOST_DIR}/lib)

#include_directories(${BOOST_INCLUDE_DIR})

SET(BOOST_ROOT ${BOOST_DIR})

## dlib
SET(DLIB_DIR          /media/ghaith/data/development/installs/dlib/install)
SET(DLIB_INCLUDE_DIR  ${DLIB_DIR}/include)
SET(DLIB_LIB_DIR      ${DLIB_DIR}/lib) #optionnal

#########


add_subdirectory( ev )
add_subdirectory( bin )
add_subdirectory( tests )




