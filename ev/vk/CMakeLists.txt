file (GLOB VK_HEADERS *.hpp)
file (GLOB VK_SOURCES *.cpp)

add_library(ev_vk ${VK_HEADERS} ${VK_SOURCES})
set_target_properties(ev_vk PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(ev_vk vulkan)
