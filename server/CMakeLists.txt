cmake_minimum_required(VERSION 3.10)
project(Server)

add_executable(server
    main.cpp
    server.cpp
    server.hpp
)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})