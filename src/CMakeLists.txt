# Project Name
# Copyright (c) 2014 by Florian Wolz
# All Rights reserved.

#add_subdirectory(module)

add_executable(albus main.cpp)
#target_link_libraries(albus module)

#target_link_libraries(albus )

target_link_libraries(albus ${Boost_LIBRARIES})