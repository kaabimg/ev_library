cmake_minimum_required( VERSION 2.6 )



set(HEADER_FILES utils.hpp
    thread_pool.hpp
    logging.hpp
    shared_data.hpp
    preprocessor.hpp
    scope_exit.hpp
    object.hpp
    basic_types.hpp
)


add_library(ev_core STATIC ${HEADER_FILES})
set_target_properties(ev_core PROPERTIES LINKER_LANGUAGE CXX)
