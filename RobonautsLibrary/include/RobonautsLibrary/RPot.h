/*******************************************************************************
 *
 * File: RPot.cpp
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
 * potentiometer or any relatively linear analog sensor.  It also provides
 * a check to make sure the sensor is ready to be used so an unplugged or 
 * damaged sensor does not lead to problems.
 *
 *  RPot *my_pot = new RPot(my_slot, my_chan);
 *  my_pot->setScale(my_scale);
 *  my_pot->setOffset(my_offset);
 *  
 *  ...
 *  
 * 	if (my_pot->isReady())
 *	{
 *		my_mot_com = my_kp * (my_target_pos - my_pot->getPosition());
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
class RPot : public AnalogChannel
#else
class RPot : public AnalogInput
#endif
{
	public:
#if defined (FRC_CRIO)
		RPot(uint32_t slot, uint32_t channel);
#else
		RPot(uint32_t channel);
#endif
		~RPot(void);

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
		float offset;
		float scale;
		float min_raw_value;
		float max_raw_value;
};
