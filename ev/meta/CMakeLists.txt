set(HEADER_FILES
    meta.hpp
)

add_library(ev_meta STATIC ${HEADER_FILES})
set_target_properties(ev_meta PROPERTIES LINKER_LANGUAGE CXX)
