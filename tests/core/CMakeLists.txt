find_package(Boost REQUIRED system thread unit_test_framework)

function (add_simple_test name)
    add_executable ("${name}_tst" "${name}.cpp" )
    target_link_libraries ("${name}_tst" ${Boost_LIBRARIES})
endfunction()

add_executable (executor_tst executor.cpp)
target_link_libraries (executor_tst pthread ${Boost_LIBRARIES})

add_executable (synchronized_data_tst synchronized_data.cpp)
target_link_libraries (synchronized_data_tst pthread  ${Boost_LIBRARIES})

add_simple_test(property)
add_simple_test(string_utils)
add_simple_test(scope_guard)
add_simple_test(flags)
add_simple_test(lru_cache)
add_simple_test(algorithm)
add_simple_test(overloaded)

add_executable (pimpl_tst pimpl.cpp pimpl_class.hpp pimpl_class.cpp)

