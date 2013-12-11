if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Include local CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# project name
##################################################
PROJECT(CaveFiber)

# Setup pkg-config
include(FindPkgConfig)

# Setup the defaults
##################################################
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Third Party libraries
##################################################

# Setup Vrui

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:../pkgconfig/")
PKG_SEARCH_MODULE(VRUI REQUIRED Vrui-1.0 Vrui)
string(REPLACE "-framework;" "-framework " VRUI_LDFLAGS "${VRUI_LDFLAGS}")
include_directories(${VRUI_INCLUDEDIR})
set(LIBS ${LIBS} ${VRUI_LDFLAGS})
#add_definitions(${VRUI_CFLAGS})

# Setup GL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARY})

FIND_PACKAGE( GLEW REQUIRED )
INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_PATH} )
set(LIBS ${LIBS} ${GLEW_LIBRARY})

# Source/Header files
##################################################

SET(CAVEFIBER_SRC
    src/main.cpp
    src/fiber.cpp
    src/FiberApplication.cpp)
    
SET(CAVEFIBER_HDR
    src/fiber.h
    src/FiberApplication.h)

# Platform Specific Config 
##################################################
IF(UNIX)
	ADD_DEFINITIONS(-std=c++0x)
ENDIF(UNIX)

ADD_EXECUTABLE(Cave_Fiber ${CAVEFIBER_SRC} ${CAVEFIBER_HDR})

TARGET_LINK_LIBRARIES(Cave_Fiber ${LIBS})
