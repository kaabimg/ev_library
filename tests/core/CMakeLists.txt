
find_package(Boost REQUIRED system thread)

add_executable (executor executor.cpp)
target_link_libraries (executor pthread ${Boost_LIBRARIES})

add_executable (lru_cache lru_cache.cpp)

add_executable (synchronized_data synchronized_data.cpp)
target_link_libraries (synchronized_data pthread )

add_executable (flags flags.cpp)
add_executable (scope_guard scope_guard.cpp)


