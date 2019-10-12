/*******************************************************************************
 *
 * File: RAbsPosSensor.cpp
 * 
 * This file contains the definition of a class for interacting with an
 * Absolute Position Sensor.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RAbsPosSensor.h"

using namespace frc;

/******************************************************************************
 * 
 * Create an interface to an APS plugged into the specified channel.
 * 
 * @param	channel	the channel or port that the pot is in
 * 
 ******************************************************************************/
RAbsPosSensor::RAbsPosSensor(uint32_t channel)
	: AnalogInput(channel)
{
	m_scale = 1.0;
	m_offset = 0.0;
	
	m_wrap_count = 0;

	m_previous_raw = getRaw();
	m_raw_range = 5.0;
}

/******************************************************************************
 *
 ******************************************************************************/
RAbsPosSensor::~RAbsPosSensor(void)
{
}

/******************************************************************************
 * 
 * Get the raw value from the sensor port. This method is also responsible
 * for tracking the number of times the sensor wraps so it should be called
 * at a period that will get a sensor value at least every quarter turn of the
 * sensor.
 * 
 * @return  the raw value from the analog input
 *
 ******************************************************************************/
float RAbsPosSensor::getRaw(void)
{
	float raw_value = AnalogInput::GetVoltage();

	if ((m_previous_raw > (m_raw_range * 0.75)) && (raw_value < (m_raw_range * 0.25)))
	{
		m_wrap_count++;
	}
	else if ( (m_previous_raw < (m_raw_range * 0.25)) && (raw_value > (m_raw_range * 0.75)))
	{
		m_wrap_count--;
	}

	m_previous_raw = raw_value;

	return raw_value;
}

/******************************************************************************
 *
 * Gets the calibrated value from the sensor. This value will wrap as the
 * sensor wraps. This method calls getRaw()
 *
 * @see getRaw()
 * @see getPositionWithWraps()
 *
 * @return  the calibrated value, units are dependent on scale and offset
 *
 ******************************************************************************/
float RAbsPosSensor::getPosition(void)
{
	return (getRaw() * m_scale) + m_offset;
}

/******************************************************************************
 * 
 * Gets the calibrated value from the sensor taking into account the number of
 * times the sensor wrapped. This method calls getRaw()
 *
 * @see getRaw()
 * @see getPosition()
 *
 * @return  the calibrated value, units are dependent on scale and offset
 *
 ******************************************************************************/
float RAbsPosSensor::getPositionWithWraps(void)
{
	return ((getRaw() + (m_wrap_count * m_raw_range)) * m_scale) + m_offset;
}

/******************************************************************************
 * 
 * @return  the number of times the sensor wrapped
 * 
 ******************************************************************************/
int RAbsPosSensor::getWrapCount(void)
{
	return m_wrap_count;
}

/******************************************************************************
 * 
 * set the number of times the sensor wrapped, so caller can put into a range
 *
 ******************************************************************************/
void RAbsPosSensor::moveWrapCountToRange(float min, float max)
{
	int max_cnt = 10;

	while ((getPositionWithWraps() < min) && (max_cnt-- > 0))
	{
		if (((getRaw() + ((m_wrap_count + 1) * m_raw_range)) * m_scale) + m_offset > getPositionWithWraps())
		{
			m_wrap_count++;
		}
		else
		{
			m_wrap_count--;
		}
	}

	max_cnt = 10;
	while ((getPositionWithWraps() > max) && (max_cnt-- > 0))
	{
		if (((getRaw() + ((m_wrap_count + 1) * m_raw_range)) * m_scale) + m_offset > getPositionWithWraps())
		{
			m_wrap_count--;
		}
		else
		{
			m_wrap_count++;
		}
	}
}

/******************************************************************************
 *
 * Gets voltage from pot and changes to position
 * 
 * @return  the calibrated value, units are dependant on scale and 
 * 			offset
 * 
 ******************************************************************************/
float RAbsPosSensor::getScale(void)
{
	return m_scale;
}

/******************************************************************************
 * 
 * Gets voltage from pot and changes to position
 * 
 * @return  the calibrated value, units are dependant on scale and 
 * 			offset
 * 
 ******************************************************************************/
float RAbsPosSensor::getOffset(void)
{
	return m_offset;
}

/******************************************************************************
 * 
 * Sets the scale for this potentiometer, this sets the ratio of the
 * raw value and te calibrated value.
 * 
 * @param	val	the new scale for this potentiometer, default 1.0
 * 
 ******************************************************************************/
void RAbsPosSensor::setScale(float val)
{
	m_scale = val;
}

/******************************************************************************
 * 
 * Sets the offset for this potentiometer in the calibrated units
 * 
 * @param	val	the new offset in the calibrated units, default 0.0.
 * 
 ******************************************************************************/
void RAbsPosSensor::setOffset(float val)
{
	m_offset = val;
}

/******************************************************************************
 *
 * Sets the raw value range so the value can be tracked across sensor wraps.
 * This value defaults to 5.0 (volts) and will likely never need to be changed.
 *
 * @param	val	the raw value range, default 5.0.
 *
 ******************************************************************************/
void RAbsPosSensor::setRawRange(float arg)
{
	m_raw_range = arg;
}

/******************************************************************************
 *
 * @return	the raw value range
 *
 ******************************************************************************/
float RAbsPosSensor::getRawRange(void)
{
	return m_raw_range;
}
