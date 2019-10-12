/*******************************************************************************
 *
 * File: ScaledAnalog.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#if defined (FRC_CRIO)
#include "AnalogChannel.h"
#elif _2018
#include "AnalogInput.h"
#else
#include "frc/AnalogInput.h"
#endif

using namespace frc;

/*******************************************************************************
 *
 * This class provides an interface for getting calibrated values from a 
 * scaled analog sensor or any relatively linear analog sensor.  It also
 * checks to make sure the sensor is ready to be used. Therefore, an unplugged or
 * damaged sensor does not lead to problems.
 *
 *  ScaledAnalog *my_scaled_analog = new ScaledAnalog(my_slot, my_chan);
 *  my_scaled_analog->setScale(my_scale);
 *  my_scaled_analog->setOffset(my_offset);
 *  
 *  ...
 *  
 * 	if (my_scaled_analog->isReady())
 *	{
 *		my_mot_com = my_kp * (my_target_pos - my_scaled_analog->getPosition());
 *	}
 *	else
 *	{
 *		my_mot_com = 0.0;
 *	}
 *	
 *	my_mot->set(my_mot_com);
 * 
 * 
 ******************************************************************************/
#if defined (FRC_CRIO)
class ScaledAnalog : public AnalogChannel
#else
class ScaledAnalog : public AnalogInput
#endif
{
	public:
#if defined (FRC_CRIO)
	ScaledAnalog(uint32_t slot, uint32_t channel);
#else
	ScaledAnalog(uint32_t channel);
#endif
		~ScaledAnalog(void);

		bool isReady(void);
		float getPosition(void);
		float getRaw(void);
		float getScale(void);
		float getOffset(void);
		
		void setScale(float scale);
		void setOffset(float offset);
		void setMinimumRawValue(float val);
		void setMaximumRawValue(float val);

	private:
		float m_offset;
		float m_scale;
		float m_min_raw_value;
		float m_max_raw_value;
};
