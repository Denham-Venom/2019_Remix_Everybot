cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(PROJECT_NAME RobonautsLibrary)

message(STATUS "************  ${PROJECT_NAME} ************")
project(${PROJECT_NAME})


set SRCS(
	src/BPid.cpp
	src/DataLogger.cpp
	src/MacroController.cpp
	src/Macro.cpp
	src/MacroStep.cpp
	src/MacroStepFactory.cpp
	src/OIController.cpp
	src/OICypress.cpp
	src/OIDevice.cpp
	src/OIDriverStation.cpp
	src/OIGenericHID.cpp
	src/OIJoystick.cpp
	src/OIObserver.cpp
	src/Parameter.cpp
	src/PeriodicControl.cpp
	src/RCounter.cpp
	src/RGyro.cpp
	src/RobotUtil.cpp
	src/RPot.cpp
	src/SimplePID.cpp
	src/SimpleTrapCntl.cpp
	src/XmlRobotUtil.cpp
		)

set HDRS(
	include/RobonautsLibrary/BPid.h
	include/RobonautsLibrary/DataLogger.h
	include/RobonautsLibrary/MacroController.h
	include/RobonautsLibrary/Macro.h
	include/RobonautsLibrary/MacroStepFactory.h
	include/RobonautsLibrary/MacroStep.h
	include/RobonautsLibrary/OIController.h
	include/RobonautsLibrary/OICypress.h
	include/RobonautsLibrary/OIDevice.h
	include/RobonautsLibrary/OIDriverStation.h
	include/RobonautsLibrary/OIGenericHID.h
	include/RobonautsLibrary/OIJoystick.h
	include/RobonautsLibrary/OIObserver.h
	include/RobonautsLibrary/Parameter.h
	include/RobonautsLibrary/PeriodicControl.h
	include/RobonautsLibrary/RCounter.h
	include/RobonautsLibrary/RGyro.h
	include/RobonautsLibrary/RobotUtil.h
	include/RobonautsLibrary/RPot.h
	include/RobonautsLibrary/SimplePID.h
	include/RobonautsLibrary/SimpleTrapCntl.h
	include/RobonautsLibrary/XmlRobotUtil.h
		)


include_directories(include)

add_library(${PROJECT_NAME} ${HDRS} ${SRCS})
