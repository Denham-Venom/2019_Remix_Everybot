/*******************************************************************************
 *
 * File: OIJoystick.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/Advisory.h"
#include "RobonautsLibrary/OIJoystick.h"
#include "RobonautsLibrary/OIObserver.h"

#ifdef _2018
#include "Joystick.h"
#else
#include "frc/Joystick.h"
#endif

#include <stdio.h>

using namespace frc;

/*******************************************************************************	
 * 
 * Create an instance of a Joystick or Gamepad controller
 * 
 * @param	name	the name of this joystick, just for debugging clarity
 * @param	device_id	the joystick number  (1 thru 4)
 * 
 ******************************************************************************/
OIJoystick::OIJoystick(std::string name, int device_id) : 
	OIDevice(name, 4, 12, 1)  // 4 analog, 12 digital, 1 pov.
{
	Advisory::pinfo("    creating joystick %d", device_id);
	stick = new Joystick(device_id);
}

/*******************************************************************************	
 *
 * Release any resources allocated by an instance of this class
 * 
 ******************************************************************************/
OIJoystick::~OIJoystick(void)
{
	if (stick != NULL)
	{
		delete stick;
		stick = NULL;
	}
}

/*******************************************************************************	
 *
 * override the oiddevice class to handle 1-based joystick values
 * 
 ******************************************************************************/
void OIJoystick::subscribeDigital(int chan, OIObserver *obs, int obs_data, 
    bool obs_invert)
{
    OIDevice::subscribeDigital(chan-1, obs, obs_data, obs_invert);
}

/*******************************************************************************	
 *
 * Check all of the input channels, if any of them changed notify the 
 * observer of the change
 * 
 ******************************************************************************/
void OIJoystick::update(void)
{
	for (int i = 0; i < ANALOG_CHANNELS; i++)
	{
		updateAnalog(i, stick->GetRawAxis(i));
	}

	for (int i = 0; i < DIGITAL_CHANNELS; i++)
	{
		updateDigital(i, stick->GetRawButton(i+1)); // buttons are 1 based
	}

	for (int i = 0; i < INT_CHANNELS; i++)
	{
		updateInt(i, stick->GetPOV(i));
	}

}
