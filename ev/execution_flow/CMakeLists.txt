set(HEADER_FILES
    graph.hpp
    node.hpp
)
add_library(ev_execution_flow STATIC ${HEADER_FILES})
set_target_properties(ev_execution_flow PROPERTIES LINKER_LANGUAGE CXX)
