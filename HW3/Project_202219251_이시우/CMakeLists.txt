cmake_minimum_required(VERSION 3.16)

project(BusanMetroRoute LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    add_executable(BusanMetroRoute src/main.cpp resources.qrc)
    target_link_libraries(BusanMetroRoute PRIVATE Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    add_executable(BusanMetroRoute src/main.cpp resources.qrc)
    target_link_libraries(BusanMetroRoute PRIVATE Qt5::Widgets)
endif()
