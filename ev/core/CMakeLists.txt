set(HEADER_FILES
    utils.hpp
    file_utils.hpp
    thread_pool.hpp
    logging.hpp
    logging_helpers.hpp
    synchronized_data.hpp
    preprocessor.hpp
    scope_exit.hpp
    object.hpp
    flags.hpp
    basic_types.hpp
    property.hpp
)


add_library(ev_core STATIC ${HEADER_FILES})
set_target_properties(ev_core PROPERTIES LINKER_LANGUAGE CXX)
