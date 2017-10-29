find_package(Boost REQUIRED system thread)

add_executable (executor_tst executor.cpp)
target_link_libraries (executor_tst pthread ${Boost_LIBRARIES})

add_executable (lru_cache_tst lru_cache.cpp)

add_executable (synchronized_data_tst synchronized_data.cpp)
target_link_libraries (synchronized_data_tst pthread )

add_executable (flags_tst flags.cpp)
add_executable (scope_guard_tst scope_guard.cpp)
add_executable (string_utils_tst string_utils.cpp)
