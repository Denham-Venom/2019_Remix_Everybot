/*******************************************************************************
 *
 * File: RPot.cpp
 * 
 * This ile contains the defintion of a class for interacting with a simple
 * potentiometer.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RPot.h"

using namespace frc;

/******************************************************************************
 * 
 * Create an interface to a potentiometer plugged into the 
 * specified slot and channel.
 * 
 * @param	slot	the module or slot for the analog card (FRC_CRIO only)
 * @param	channel	the channel or port that the pot is in
 * 
 ******************************************************************************/
RPot::RPot(uint32_t channel) :
	AnalogInput(channel)
{
	scale = 1.0;
	offset = 0.0;
	
	min_raw_value = 0.1;
	max_raw_value = 4.9;
}

/******************************************************************************
 *
 * This destructor does nothing
 *
 ******************************************************************************/
RPot::~RPot(void)
{
}

/******************************************************************************
 * 
 * Check to see if the sensor is ready to be used.  If the sensor is
 * unplugged or shorted out it should read a raw value of either 0.0
 * or max voltage.  Operational limits may be more restrictive.
 * 
 * @return	true if the raw value is between the minimum and maximum
 * 			specified raw values.
 * 			
 ******************************************************************************/
bool RPot::isReady(void)
{
	float volts = getRaw();
	return ((volts >= min_raw_value) && (volts <= max_raw_value));
}

/******************************************************************************
 *
 * Gets voltage from pot and changes to position
 *
 * @return  the calibrated value, units are dependent on scale and
 * 			offset
 *
 ******************************************************************************/
float RPot::getRaw(void)
{
	return AnalogInput::GetVoltage();
}

/******************************************************************************
 * 
 * Gets voltage from pot and changes to position
 * 
 * @return  the calibrated value, units are dependent on scale and
 * 			offset
 * 
 ******************************************************************************/
float RPot::getPosition(void) 
{
	return (getRaw() * scale) + offset;
}

/******************************************************************************
 * 
 * Gets voltage from pot and changes to position
 * 
 * @return  the calibrated value, units are dependent on scale and
 * 			offset
 * 
 ******************************************************************************/
float RPot::getScale(void) 
{
	return scale;
}

/******************************************************************************
 * 
 * Gets voltage from pot and changes to position
 * 
 * @return  the calibrated value, units are dependent on scale and
 * 			offset
 * 
 ******************************************************************************/
float RPot::getOffset(void) 
{
	return offset;
}

/******************************************************************************
 * 
 * Sets the scale for this potentiometer, this sets the ratio of the
 * raw value and te calibrated value.
 * 
 * @param	val	the new scale for this potentiometer, default 1.0
 * 
 ******************************************************************************/
void RPot::setScale(float val) 
{
	scale = val;
}

/******************************************************************************
 * 
 * Sets the offset for this potentiometer in the calibrated units
 * 
 * @param	val	the new offset in the calibrated units, default 0.0.
 * 
 ******************************************************************************/
void RPot::setOffset(float val) 
{
	offset = val;
}

/******************************************************************************
 * 
 * Set the minimum raw value that will be considered as valid in the
 * isReady check.
 * 
 * @param	val	the new minimum raw value (volts), default 0.1.
 * 
 ******************************************************************************/
void RPot::setMinimumRawValue(float val)
{
	min_raw_value = val;
}

/******************************************************************************
 * 
 * Set the maximum raw value that will be considered as valid in the
 * isReady check.
 * 
 * @param	val	the new minimum raw value (volts), default 4.9.
 * 
 ******************************************************************************/
void RPot::setMaximumRawValue(float val)
{
	max_raw_value = val;
}
