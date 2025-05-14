cmake_minimum_required(VERSION 3.10)
project(Server)

#find_package(Boost REQUIRED COMPONENTS system)

add_executable(server
    main.cpp
    server.cpp
    server.hpp
)

#target_link_libraries(server ${Boost_LIBRARIES} pthread)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})