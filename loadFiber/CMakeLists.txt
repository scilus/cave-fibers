if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# project name
##################################################
PROJECT(CaveFiber)

# Source/Header files
##################################################

SET(CAVEFIBER_SRC
    src/main.cpp
    src/fiber.cpp)
    
SET(CAVEFIBER_HDR
    src/fiber.h)

IF(UNIX)
	ADD_DEFINITIONS(-std=c++0x)
ENDIF(UNIX)

ADD_EXECUTABLE(Cave_Fiber ${CAVEFIBER_SRC} ${CAVEFIBER_HDR})
