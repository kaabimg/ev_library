cmake_minimum_required( VERSION 2.6 )

project(ev_library)






find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")


SET(ROOT_DIR   ${PROJECT_SOURCE_DIR}/)
SET(INSTALL_DIR  ${ROOT_DIR}/install)

SET(BIN_DIR    ${INSTALL_DIR}/bin)
SET(LIB_DIR    ${INSTALL_DIR}/lib)
SET(TEST_DIR   ${INSTALL_DIR}/test)


include_directories(${PROJECT_SOURCE_DIR})





#### Libraries

## boost

SET(BOOST_DIR         /home/ghaith/data/development/installs/boost/install_1_62_0)
SET(BOOST_INCLUDE_DIR ${BOOST_DIR}/include)
SET(BOOST_LIB_DIR     ${BOOST_DIR}/lib)

#include_directories(${BOOST_INCLUDE_DIR})

SET(BOOST_ROOT ${BOOST_DIR})



#########


add_subdirectory( ev )
add_subdirectory( tests )




