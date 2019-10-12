/*******************************************************************************
 *
 * File: OIJoystick.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "OIGenericHID.h"

#ifdef _2018
#include "Joystick.h"
#else
#include "frc/Joystick.h"
#endif

#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIDevice.h"

using namespace frc;

/*******************************************************************************
 *
 * This OI Device creates an event dispatcher for monitoring the buttons and
 * dials on a HID.
 *
 ******************************************************************************/
class OIGenericHID : public OIDevice
{
	public:
		OIGenericHID(std::string name, int analogs, int digitals, int povs, int device_id);
		~OIGenericHID(void);

		void update(void);

	protected:
		Joystick *stick;
};
