/*******************************************************************************
 *
 * File: OIGenericHID.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/Advisory.h"

#include "RobonautsLibrary/OIGenericHID.h"
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
 * @param	name		the name of this joystick, just for debugging clarity
 * @param	digitals 	the number of digital channels.
 * @param	analogs		the number of analog channels.
 * @param 	povs		the number of pov channelz.
 * @param	device_id	the joystick number  (1 thru 4)
 * 
 ******************************************************************************/
OIGenericHID::OIGenericHID(std::string name, int analogs, int digitals,
	int povs, int device_id) :
	OIDevice(name, analogs, digitals, povs),
	stick(NULL)
{
	Advisory::pinfo("    creating generic HID %d", device_id);

	try
	{
		stick = new Joystick(device_id);
	}
	catch (...)
	{
		stick = NULL;
		Advisory::pinfo("    failed to initialize stick of type Joystick.");
	}
}


/*******************************************************************************	
 *
 * Release any resources allocated by an instance of this class
 * 
 ******************************************************************************/
OIGenericHID::~OIGenericHID(void)
{
	if (stick != NULL)
	{
		delete stick;
		stick = NULL;
	}
}

/*******************************************************************************	
 *
 * Check all of the input channels, if any of them changed notify the 
 * observer of the change.
 * 
 ******************************************************************************/
void OIGenericHID::update(void)
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
