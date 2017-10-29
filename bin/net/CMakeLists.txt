
set(ZMQ_ROOT /media/ghaith/data/development/installs/zeromq-4.2.1)

find_package(Boost REQUIRED system thread)

include_directories(${ZMQ_ROOT}/include)

LINK_DIRECTORIES(${ZMQ_ROOT}/build/lib)

add_executable (net net.cpp)
target_link_libraries(net zmq-static ev_net ${Boost_LIBRARIES})
