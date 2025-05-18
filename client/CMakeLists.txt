cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Quick
    Qml
    Network
    QuickControls2
)

qt5_add_resources(QML_QRC qml.qrc)

add_executable(client
    main.cpp
    client.cpp
    client.hpp
    ${QML_QRC}
)

target_link_libraries(client
    Qt5::Quick
    Qt5::Qml
    Qt5::Network
    Qt5::QuickControls2
)
