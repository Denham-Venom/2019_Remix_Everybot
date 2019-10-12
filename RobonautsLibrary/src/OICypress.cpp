/*******************************************************************************
 *
 * File: OICypress.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#if defined (FRC_CRIO)
#include "math.h"

#include "Joystick.h"

#include "RobonautsLibrary/OIJoystick.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OICypress.h"

/*******************************************************************************	
 * 
 * Create an instance of a Cypress board
 * 
 ******************************************************************************/
OICypress::OICypress(std::string name, DriverStation *ds) :
	OIDevice(name, 8, 16)  // 8 analogs, 16 digitals
{
	Advisory::pinfo("Creating Cypress %s", name.c_str());
	this-> ds = ds;
	enhanced_ds = &ds->GetEnhancedIO();
}

/*******************************************************************************	
 *
 * Release any resources allocated by an instance of this class
 * 
 ******************************************************************************/
OICypress::~OICypress(void)
{
	if (ds != NULL)
	{
		ds = NULL;
	}
	if (enhanced_ds != NULL)
	{
		enhanced_ds = NULL;
	}
}

/*******************************************************************************	
 *
 * Check all of the input channels, if any of them changed notify the 
 * observer of the change
 * 
 ******************************************************************************/
void OICypress::update(void)
{
	for (int i = 0; i < ANALOG_CHANNELS; i++)
	{
		updateAnalog(i, enhanced_ds->GetAnalogIn(i + 1));
	}

	for (int i = 0; i < DIGITAL_CHANNELS; i++)
	{
		updateDigital(i, enhanced_ds->GetDigital(i + 1));
	}
}

/*******************************************************************************	
 * 
 * Update all the specified channel with a new value.  If the new value is 
 * different from the previous value, the observers will be notified.
 * 
 * @param	idx		the channel index (zero based) of the value being updated
 * 
 * @param	val		the new value of the specified channel
 * 						
 ******************************************************************************/
void OICypress::updateAnalog(int idx, float val)
{
	if (fabs(analog_val[idx] - val) > 0.001) // if the value changed by enough
	{
		OIDeviceObserver *temp_obs = analog_observers[idx];
		
		while(temp_obs != NULL)
		{
			temp_obs->obs->setAnalog(temp_obs->obs_data, val * temp_obs->adjust.scale);
			
			temp_obs = (OIDeviceObserver *)(temp_obs->next);
		}

		analog_val[idx] = val;
	}
}

#endif
