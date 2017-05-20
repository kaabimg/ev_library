file (GLOB HEADERS *.hpp)
#file (GLOB SOURCES *.cpp)

add_library(ev_vk STATIC ${HEADERS})
set_target_properties(ev_vk PROPERTIES LINKER_LANGUAGE CXX)
