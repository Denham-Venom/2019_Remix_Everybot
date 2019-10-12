/*******************************************************************************
 *
 * File: ScaledAnalog.cpp
 * 
 * 	This class provides an interface for getting calibrated values from a
 * scaled analog sensor or any relatively linear analog sensor.  It also
 * checks to make sure the sensor is ready to be used. Therefore, an unplugged or
 * damaged sensor does not lead to problems.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/ScaledAnalog.h"

using namespace frc;

/******************************************************************************
 * 
 * Create an interface to a scaled analog sensor plugged into the
 * specified slot and channel.
 * 
 * @param	slot	the module or slot for the analog card (FRC_CRIO only)
 * @param	channel	the channel or port that the scaled analog is in
 * 
 ******************************************************************************/
#if defined (FRC_CRIO)
ScaledAnalog::ScaledAnalog(uint32_t slot, uint32_t channel) :
	AnalogChannel(slot, channel)
#else
ScaledAnalog::ScaledAnalog(uint32_t channel) :
	AnalogInput(channel)
#endif
{
	m_scale = 1.0;
	m_offset = 0.0;
	
	m_min_raw_value = 0.1;
	m_max_raw_value = 4.9;
}

/******************************************************************************
 *
 ******************************************************************************/
ScaledAnalog::~ScaledAnalog(void)
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
bool ScaledAnalog::isReady(void)
{
	float volts = getRaw();
	return ((volts >= m_min_raw_value) && (volts <= m_max_raw_value));
}

/******************************************************************************
 *
 * Gets voltage from the scaled analog and changes to position
 *
 * @return  the calibrated value, units are dependant on scale and
 * 			offset
 *
 ******************************************************************************/
float ScaledAnalog::getRaw(void)
{
#if defined (FRC_CRIO)
	return GetVoltage();
#else
//	return GetVoltage();
	return AnalogInput::GetVoltage();
#endif
}

/******************************************************************************
 * 
 * Gets voltage from scaled analog and changes to position
 * 
 * @return  the calibrated value, units are dependant on scale and 
 * 			offset
 * 
 ******************************************************************************/
float ScaledAnalog::getPosition(void)
{
	return (getRaw() * m_scale) + m_offset;
}

/******************************************************************************
 * 
 * Gets voltage from scaled analog and changes to position
 * 
 * @return  the calibrated value, units are dependant on scale and 
 * 			offset
 * 
 ******************************************************************************/
float ScaledAnalog::getScale(void)
{
	return m_scale;
}

/******************************************************************************
 * 
 * Gets voltage from scaled analog and changes to position
 * 
 * @return  the calibrated value, units are dependant on scale and 
 * 			offset
 * 
 ******************************************************************************/
float ScaledAnalog::getOffset(void)
{
	return m_offset;
}

/******************************************************************************
 * 
 * Sets the scale for this scaled analog, this sets the ratio of the
 * raw value and the calibrated value.
 * 
 * @param	val	the new scale for this scaled analog, default 1.0
 * 
 ******************************************************************************/
void ScaledAnalog::setScale(float val)
{
	m_scale = val;
}

/******************************************************************************
 * 
 * Sets the offset for this scaled analog in the calibrated units
 * 
 * @param	val	the new offset in the calibrated units, default 0.0.
 * 
 ******************************************************************************/
void ScaledAnalog::setOffset(float val)
{
	m_offset = val;
}

/******************************************************************************
 * 
 * Set the minimum raw value that will be considered as valid in the
 * isReady check.
 * 
 * @param	val	the new minimum raw value (volts), default 0.1.
 * 
 ******************************************************************************/
void ScaledAnalog::setMinimumRawValue(float val)
{
	m_min_raw_value = val;
}

/******************************************************************************
 * 
 * Set the maximum raw value that will be considered as valid in the
 * isReady check.
 * 
 * @param	val	the new minimum raw value (volts), default 4.9.
 * 
 ******************************************************************************/
void ScaledAnalog::setMaximumRawValue(float val)
{
	m_max_raw_value = val;
}
