# Project Name
# Copyright (C) 2014 Florian Wolz
#
# All rights reserved.

PROJECT (CONSTRUCTION)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_BUILD_TYPE Debug)

add_definitions(-std=c++11 -O3 -g -lpthread)

set(CONSTRUCTION_MAIN_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CONSTRUCTION_LIBRARY_DIRECTORY "${CONSTRUCTION_MAIN_DIRECTORY}/lib")
set(CONSTRUCTION_SOURCE_DIRECTORY "${CONSTRUCTION_MAIN_DIRECTORY}/src")
set(CONSTRUCTION_EXTERNAL_DIRECTORY "${CONSTRUCTION_MAIN_DIRECTORY}/external")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")
set(CMAKE_MODULE_PATH "${CONSTRUCTION_MAIN_DIRECTORY}/cmake")

# Use boost
find_package(Boost COMPONENTS system filesystem serialization iostreams REQUIRED)
find_package(Threads REQUIRED)
find_package(Readline REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Readline_INCLUDE_DIR})

# Documentation
#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#    add_custom_target(doc
#            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#            COMMENT "Generating API documentation with Doxygen" VERBATIM
#    )
#endif(DOXYGEN_FOUND)

# Include the headers
include_directories(${CONSTRUCTION_MAIN_DIRECTORY})

include_directories(
    ${CONSTRUCTION_LIBRARY_DIRECTORY}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")

add_subdirectory(src)

# Testing
add_subdirectory(test)
