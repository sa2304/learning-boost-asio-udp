cmake_minimum_required(VERSION 3.23)
project(udp)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system coroutine)

add_executable(client client.cpp)
target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS}})
target_link_directories(client PRIVATE ${Boost_LIBRARY_DIRS})
target_link_libraries(client PRIVATE ${Boost_LIBRARIES})

add_executable(server server.cpp)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS}})
target_link_directories(server PRIVATE ${Boost_LIBRARY_DIRS})
target_link_libraries(server PRIVATE ${Boost_LIBRARIES})

