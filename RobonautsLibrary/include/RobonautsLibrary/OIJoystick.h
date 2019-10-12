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
 * dials on a joystick or gamepad.
 *
 ******************************************************************************/
class OIJoystick : public OIDevice
{
	public:
		OIJoystick(std::string name, int device_id);
		~OIJoystick(void);

		void update(void);

		void subscribeDigital(int chan, OIObserver *obs, int obs_data, bool obs_invert);

	protected:
		Joystick *stick;
};
