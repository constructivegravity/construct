# Construction
# Copyright (c) 2016 by Florian Wolz
# All Rights reserved.
#
# Module: Tensor
#         Module for all the tensor arithmetics

set(CONSTRUCTION_TENSOR_HEADERS

)

set(CONSTRUCTION_TENSOR_SOURCE
	scalar.cpp
)

add_library(tensor SHARED ${CONSTRUCTION_TENSOR_SOURCE})
