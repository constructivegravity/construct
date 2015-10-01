# Project Name
# Copyright (C) 2014 Florian Wolz
#
# All rights reserved.

PROJECT (albus)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

add_definitions(-std=c++11)

set(ALBUS_MAIN_DIRECTORY ${PROJECT_SOURCE_DIR})
set(ALBUS_LIBRARY_DIRECTORY "${ALBUS_MAIN_DIRECTORY}/lib")
set(ALBUS_SOURCE_DIRECTORY "${ALBUS_MAIN_DIRECTORY}/src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")

find_package(Boost COMPONENTS system serialization REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

include_directories(
    ${ALBUS_LIBRARY_DIRECTORY}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")

add_subdirectory(src)
