/*******************************************************************************
 *
 * File: RobotMain.cpp
 *
 * Written by:   
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#ifdef _2018
#include "WPILib.h"
#else
// 2019 Changed 
#include "frc/WPILib.h"
#endif

#include "RobotMain.h"

using namespace frc;

/******************************************************************************
 *
 *
 *
 ******************************************************************************/
RobotMain::RobotMain(void) : PacbotBase()
{
    RegisterControls();
}

/******************************************************************************
 *
 *
 *
 ******************************************************************************/
RobotMain::~RobotMain()
{
}

//
// This pre-processor macro is what makes this class the class that is run
// as a robot main.
//
#ifdef _2018
START_ROBOT_CLASS(RobotMain)
#else
// 2019 Changed 
    #ifndef RUNNING_FRC_TESTS
    int main() { return frc::StartRobot<RobotMain>(); }
    #endif
#endif
