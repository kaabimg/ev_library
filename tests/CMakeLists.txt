
set(EXECUTABLE_OUTPUT_PATH ${TEST_DIR})
set(CATCH_DIR "/home/ghaith/data/development/install/catch/Catch/include/")
include_directories(${CATCH_DIR})

add_subdirectory( core )
add_subdirectory( vm )


