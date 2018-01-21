set(HEADER_FILES
    algorithm.hpp
    allocator.hpp
    utils.hpp
    file_utils.hpp
    executor.hpp
    logging.hpp
    logging_helpers.hpp
    synchronized_data.hpp
    preprocessor.hpp
    scope_guard.hpp
    flags.hpp
    basic_types.hpp
    property.hpp
    lru_cache.hpp
    overloaded.hpp
    string_utils.hpp
    thread.hpp
    traits.hpp
    meta.hpp
    pimpl.hpp
    execution_flow.hpp
    )

add_library(ev_core STATIC ${HEADER_FILES})
set_target_properties(ev_core PROPERTIES LINKER_LANGUAGE CXX)
