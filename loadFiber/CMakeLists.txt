if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# project name
##################################################
PROJECT(loadFiber)

# Source/Header files
##################################################

file( GLOB_RECURSE LOADFIBER_SRC "src/*.cpp")

file( GLOB_RECURSE LOADFIBER_HDR "src/*.h")

IF(UNIX)
	ADD_DEFINITIONS(-std=c++0x)
ENDIF(UNIX)

ADD_EXECUTABLE(testloadFiber ${LOADFIBER_SRC} ${LOADFIBER_HDR})
