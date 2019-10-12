/*******************************************************************************
 *
 * File: OIObserver.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <string>

/*******************************************************************************
 *
 * This OI Observer is a pure virtual class that acts as an interface for
 * any class that needs to be notified when the one or more of the inputs
 * from the Operator interface changes.
 *
 ******************************************************************************/
class OIObserver
{
	public:
		enum PovDirection
		{
			POV_NORTH = 0,
			POV_NORTHEAST = 45,
			POV_EAST = 90,
			POV_SOUTHEAST = 135,
			POV_SOUTH = 180,
			POV_SOUTHWEST = 225,
			POV_WEST = 270,
			POV_NORTHWEST = 315
		};

		OIObserver();
		virtual ~OIObserver();
		virtual void setAnalog(int id, float val) {}
		virtual void setInt(int id, int val) {}
		virtual void setDigital(int id, bool val) {}
		virtual void setString(int id, std::string val) {}
};
