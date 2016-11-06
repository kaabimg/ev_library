cmake_minimum_required( VERSION 2.6 )



set(HEADER_FILES utils.h
    thread_pool.h
    logging.h
    shared_data.h
    preprocessor.h
    scope_exit.h
    object.h
    basic_types.h
)


add_library(ev_core STATIC ${HEADER_FILES})
set_target_properties(ev_core PROPERTIES LINKER_LANGUAGE CXX)
