cmake_minimum_required( VERSION 2.6 )


set(EXECUTABLE_OUTPUT_PATH ${TEST_DIR})
add_executable (core_tests core.cpp)


set(CATCH_DIR "/home/ghaith/data/development/installs/Catch/Catch/include/")

include_directories(${CATCH_DIR})

target_link_libraries (core_tests pthread)

