set(HEADER_FILES
    algorithm.hpp
    utils.hpp
    file_utils.hpp
    executor.hpp
    logging.hpp
    logging_helpers.hpp
    synchronized_data.hpp
    preprocessor.hpp
    scope_guard.hpp
    object.hpp
    flags.hpp
    basic_types.hpp
    property.hpp
    lru_cache.hpp
    overloaded.hpp
    string_utils.hpp
    thread.hpp
)

add_library(ev_core STATIC ${HEADER_FILES})
set_target_properties(ev_core PROPERTIES LINKER_LANGUAGE CXX)
