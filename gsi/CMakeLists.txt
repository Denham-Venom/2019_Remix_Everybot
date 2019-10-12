cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(PROJECT_NAME gsi)
message(STATUS "************  ${PROJECT_NAME} ************")
project(${PROJECT_NAME})

#this flag is compiler dependent
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions( /wd4996 )
endif()

file (GLOB SRCS  "src/*.cpp")
file (GLOB HDRS  "include/gsi/*.h")

source_group("include" FILES ${HDRS})

include_directories(include)

add_library(${PROJECT_NAME} ${HDRS} ${SRCS})

#install(TARGETS ${PROJECT_NAME} CONFIGURATIONS Debug Release ARCHIVE DESTINATION bin)
