/*******************************************************************************
 *
 * File: RobotMain.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "RobonautsControls/PacbotBase.h"

/*******************************************************************************
 *
 * The main class for this robot controls the creation of the robot, loading
 * the configuration from file, and controlling the transition between
 * operation phases.
 * 
 ******************************************************************************/
class RobotMain : public PacbotBase
{
    public:
 		RobotMain(void);
		~RobotMain();
};
