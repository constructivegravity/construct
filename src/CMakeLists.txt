# Project Name
# Copyright (c) 2014 by Florian Wolz
# All Rights reserved.

add_subdirectory(tensor)

add_executable(construct main.cpp)
target_link_libraries(construct tensor)

#target_link_libraries(Construction )

target_link_libraries(construct ${Boost_LIBRARIES} ${Readline_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})


add_executable(coefficients coefficients.cpp)
#target_link_libraries(Construction module)

#target_link_libraries(Construction )

target_link_libraries(coefficients ${Boost_LIBRARIES} ${Readline_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
