/*******************************************************************************
 *
 * File: RGyro.h
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
#include "AnalogGyro.h"
#include "Timer.h"
#else
#include "frc/AnalogGyro.h"
#include "frc/Timer.h"
#endif
#include "RobonautsLibrary/RobotUtil.h"

/*******************************************************************************
 *
 * This class is a wrapper around the WPI Gyro class that adds two features
 * and an usage constraint.
 * 
 * First, the IsReady method can be used to help insure the Gyro is not 
 * used when it is unplugged or damaged.  Second, the Calibrate method
 * is used to continuously adjust for sesnor drift when the robot is 
 * disabled.  The constraint is that either the Calibrate method or the 
 * GetAngle method needs to be called at a regular interaval -- either call
 * Calibrate or GetAngle each cycle, do not call both or either one more than
 * once during a cycle.
 * 
 ******************************************************************************/
class RGyro : public AnalogGyro
{
	public:
		RGyro(void);
		RGyro(uint32_t channel);
		RGyro(uint32_t module, uint32_t channel);
		~RGyro(void);

		float GetAngle(void);
		float UpdateDrift(void);
		void Reset(void);
		bool IsReady(void);

	private:
		void init(void);
		float drift_rate;
		float offset;

		float cur_raw_angle;
		float prev_raw_angle;

		bool is_ready;
};
