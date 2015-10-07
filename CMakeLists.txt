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
set(ALBUS_EXTERNAL_DIRECTORY "${ALBUS_MAIN_DIRECTORY}/external")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")

# Use boost
find_package(Boost COMPONENTS system serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

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
include_directories(${ALBUS_MAIN_DIRECTORY})

include_directories(
    ${ALBUS_LIBRARY_DIRECTORY}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")

add_subdirectory(src)

# Testing
add_subdirectory(test)