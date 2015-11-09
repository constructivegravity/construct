# Project Name
# Copyright (c) 2014 by Florian Wolz
# All Rights reserved.

#add_subdirectory(module)

add_executable(construct main.cpp)
#target_link_libraries(Construction module)

#target_link_libraries(Construction )

target_link_libraries(construct ${Boost_LIBRARIES})


add_executable(coefficients coefficients.cpp)
#target_link_libraries(Construction module)

#target_link_libraries(Construction )

target_link_libraries(coefficients ${Boost_LIBRARIES})