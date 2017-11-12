set(HEADER_FILES
    neural_network.hpp
    layer.hpp
    transfert_function.hpp
    algorithm.hpp
)

add_library(ev_ml STATIC ${HEADER_FILES})
set_target_properties(ev_ml PROPERTIES LINKER_LANGUAGE CXX)
