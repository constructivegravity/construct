# Project Name
# Copyright (c) 2014 by Florian Wolz
# All Rights reserved.
#
# Module: Module name
#         A clever description

set(PROJECTNAME_MODULENAME_HEADERS
	module.hh
)

set(PROJECTNAME_MODULENAME_SOURCE
	src/module.cc
)

add_library(module SHARED ${PROJECTNAME_MODULENAME_HEADERS} ${PROJECTNAME_MODULENAME_SOURCE})