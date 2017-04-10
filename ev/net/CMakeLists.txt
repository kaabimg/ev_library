
set(ZMQ_ROOT /media/ghaith/data/development/installs/zeromq-4.2.1)

include_directories(${ZMQ_ROOT}/include)
LINK_DIRECTORIES(${ZMQ_ROOT}/build/lib)

file (GLOB HEADERS *.hpp)
file (GLOB SOURCES *.cpp)

add_library (ev_net ${HEADERS} ${SOURCES})
target_link_libraries(ev_net zmq-static)
