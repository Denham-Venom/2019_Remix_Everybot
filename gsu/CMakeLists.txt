cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(PROJECT_NAME gsu)
message(STATUS "************  ${PROJECT_NAME} ************")
project(${PROJECT_NAME})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(PORT_TYPE POSIX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(PORT_TYPE WIN)
	add_definitions( /wd4996 )
endif()

file (GLOB SRCS "src/*.cpp")
file (GLOB HDRS "include/gsu/*.h")


include_directories(include)
include_directories(../gsi/include)

add_library(${PROJECT_NAME} ${HDRS} ${SRCS})
source_group("include" FILES ${HDRS})
install(TARGETS ${PROJECT_NAME} CONFIGURATIONS Debug Release ARCHIVE DESTINATION bin)
