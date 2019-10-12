/*******************************************************************************
 *
 * File: RAbsPosSensor.h -- Absolute Position Sensor
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
#include "AnalogInput.h"
#else
#include "frc/AnalogInput.h"
#endif

using namespace frc;

/*******************************************************************************
 *
 * This class provides an interface for getting calibrated values from an
 * Absolute Position Sensor. It also provides.
 *
 *  Periodically call getRaw(), getPosition(), or getPositionWithWraps(). The
 *  period should be such that the actuation being measured cannot turn the
 *  APS more than a quarter turn during the period. This allows this class
 *  to monitor for sensor wraps.
 *
 *  RAbsPosSensor *my_aps = new RAbsPosSensor(my_chan);
 *  my_aps->setScale(my_scale);
 *  my_aps->setOffset(my_offset);
 *  
 *  ...
 *  
 *	my_pos = my_aps->getPositionWithWraps());
 * 
 ******************************************************************************/
class RAbsPosSensor : public AnalogInput
{
	public:
		RAbsPosSensor(uint32_t channel);
		~RAbsPosSensor(void);

		float getRaw(void);
		float getPosition(void);
		float getPositionWithWraps(void);
		int getWrapCount(void);
		void moveWrapCountToRange(float min, float max); // this is so the user can put into a range

		void setScale(float scale);
		float getScale(void);

		void setOffset(float offset);
		float getOffset(void);

		void setRawRange(float arg);
		float getRawRange(void);

	private:
		float m_offset;
		float m_scale;
		int   m_wrap_count;
		float m_previous_raw;
		float m_raw_range;
};
