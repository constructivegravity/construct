# Project Name
# Copyright (c) 2014 by Florian Wolz
# All Rights reserved.

add_subdirectory(tensor)

add_executable(apple main.cpp)
target_link_libraries(apple tensor ${Boost_LIBRARIES} ${Readline_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

#target_link_libraries(Construction )
