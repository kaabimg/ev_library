
set(EXECUTABLE_OUTPUT_PATH ${TEST_DIR})
set(CATCH_DIR "/media/ghaith/data/development/installs/Catch/")
include_directories("${CATCH_DIR}/include")

add_subdirectory( core )
add_subdirectory( vm )


