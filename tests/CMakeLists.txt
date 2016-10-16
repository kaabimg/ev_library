cmake_minimum_required( VERSION 2.6 )

include(../config.cmake)


set(EXECUTABLE_OUTPUT_PATH ${TEST_DIR})
add_executable (core_tests core.cpp)

TARGET_LINK_LIBRARIES( core_tests pthread )


