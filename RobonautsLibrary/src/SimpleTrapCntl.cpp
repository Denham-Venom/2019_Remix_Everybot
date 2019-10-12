/*******************************************************************************
 *
 * File: SimpleTrapCntl.cpp
 * 
 * This file contains the definition of a class that can be used as a
 * for closed loop control
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <cstdio>
#include <cmath>
#include "gsu/Advisory.h"
#include "gsi/Time.h"

#include "RobonautsLibrary/SimpleTrapCntl.h"

#include "RobonautsLibrary/RobotUtil.h"

/*******************************************************************************
 *
 * Allocate a PID object and give all instance variables default values.
 *
 ******************************************************************************/
SimpleTrapCntl::SimpleTrapCntl(void)
{
	// make sure everything gets initialized to something safe
	setControlConstants(0.0, 1.0, 0.0, 0.0);
	setControlValueLimits(-1.0, 1.0);
	setVelocityLimits(-1.0, 1.0);
	setAccelerationLimits(1.0);

	stc_error_p = 0.0; 
	stc_error_i = 0.0; 
	stc_error_d = 0.0; 
	
	stc_previous_time = gsi::Time::getTime();
	stc_previous_feedback = 0.0;
	
	stc_previous_target_vel = 0.0;
	stc_previous_feedback_vel = 0.0;
	stc_previous_decel_needed = 0.0;
	
	stc_target_pos = 0.0;

	stc_tolerance = 0.01;
	
	stc_error_i_degrade = 0.99;

	reset();
}

/*******************************************************************************
 *
 * Release all resources used by this object.
 *
 ******************************************************************************/
SimpleTrapCntl::~SimpleTrapCntl(void)
{
}

/*******************************************************************************
 *
 *  This should be called when not doing calculation so there is not a big jump
 *  when calculations start
 *
 ******************************************************************************/
void SimpleTrapCntl::resetControlValue(float targ_val, float feedback_val)
{
	stc_error_p = 0.0;
	stc_error_i = 0.0;
	stc_error_d = 0.0;

	stc_previous_target_vel = 0.0;
	stc_previous_feedback_vel = 0.0;

	stc_previous_time = gsi::Time::getTime();
	stc_previous_feedback = feedback_val;
	stc_target_pos = targ_val;
	stc_previous_decel_needed = 0.0;
}

/*******************************************************************************
 *
 * Calculate the control (manipulated) variable
 *
 ******************************************************************************/
float SimpleTrapCntl::calculateControlValue(float target, float feedback)
{
	double current_time = gsi::Time::getTime();
	double delta_time = current_time - stc_previous_time;

	float distance_to_go = target - feedback;

	// This could cause a little bit of a problem, calculating the change in
	// velocity from changes in time and position may lead to some less than smooth
	// results. Trying a running average to do a little smoothing.
	//float feedback_vel = (feedback - stc_previous_feedback) / delta_time;
    float feedback_vel = ((4.0 * stc_previous_feedback_vel) + ((feedback - stc_previous_feedback) / delta_time)) / 5.0;

    // The amount of time (t) to decelerate from a velocity(v) with an acceleration (a) is t = v/a
    // The distance (d) traveled while under constant deceleration from velocity (v) to zero is d = (v * t)/2 (it's the area of triangle)
    // With substitution the distance (d) traveled while decelerating (a) is d = (v * v) / (2 * a)
    // So deceleration needed to stop in distance is a = (v * v) / (2 * d)
    float decel_needed = 0.0;

	float target_vel = 0.0;
    if (fabs(distance_to_go) > stc_tolerance)
    {
    	decel_needed = fabs((feedback_vel * feedback_vel) / (2.0 * distance_to_go));

 //   	Advisory::pinfo("TRAP: vel=%f, targ=%f, fb=%f, dn=%f, acc=%f dtg=%f", feedback_vel, target, feedback, decel_needed, stc_accel, distance_to_go);

		if (decel_needed >= stc_accel)  // need to decelerate
		{
			target_vel = RobotUtil::rateLimit(delta_time * decel_needed, 0.0, stc_previous_target_vel);
			stc_previous_decel_needed = decel_needed;
		}
		else if (stc_previous_decel_needed >= stc_accel)
		{
			stc_previous_decel_needed = ((9.0 * stc_previous_decel_needed ) + decel_needed) / 10.0;
			target_vel = RobotUtil::rateLimit(delta_time * stc_previous_decel_needed, 0.0, stc_previous_target_vel);
		}
		else // accelerate to limit
		{
		// if not far to go, don't use max acceleration
			float step = std::min(delta_time * stc_accel, fabs(distance_to_go) / delta_time);

			// accelerate from previous target else feedback may never change if acceleration is too low
			if (distance_to_go > stc_tolerance )
			{
				target_vel = RobotUtil::rateLimit(step, stc_vel_max, stc_previous_target_vel);
			}
			else if (distance_to_go < -1.0 * stc_tolerance)
			{
				target_vel = RobotUtil::rateLimit(step, stc_vel_min, stc_previous_target_vel);
			}
			else
			{
				target_vel = 0.0;
			}
		}
    	stc_previous_decel_needed = 0.0;
    }

    // shouldn't need this, the above logic should already result in these limits.
   	target_vel = RobotUtil::limit(stc_vel_min, stc_vel_max, target_vel);

	//
	// calculate the errors
	//
	float error_cur = target_vel - feedback_vel;

	if (std::fabs((stc_previous_target_vel - target_vel)/(stc_vel_max - stc_vel_min)) > stc_threshold)
	{
		stc_error_i = 0.0;
		stc_error_d = 0.0;
	}
	else
	{
		stc_error_i += error_cur;


		// I really need to find something better than this
		// if Ei and Ep are in opposite directions Ei needs to move towards 0 fast. But not too fast
		// because if it's a temporary jump in Ep, we still need the Ei
//		if ((error_cur == 0.0)  || (RobotUtil::sign(stc_error_i) != RobotUtil::sign(error_cur)))
//		{
//			stc_error_i *= stc_error_i_degrade;
//			stc_error_i_degrade -= 0.02;
//		}
//		else
//		{
//			stc_error_i_degrade = 0.99;
//		}

		// limiting this accumulator reduces overshoot significantly
		// limiting it in this way prevents divide by zero (as long
		// as control_max is positive and control_min is negative)
		if ((stc_const_i * stc_error_i) > stc_cntl_max)
		{
			stc_error_i = stc_cntl_max / stc_const_i;
		}
		else if ((stc_const_i * stc_error_i) < stc_cntl_min)
		{
			stc_error_i = stc_cntl_min / stc_const_i;
		}

		stc_error_d = (error_cur - stc_error_p);
	}
	stc_error_p = error_cur;

	stc_previous_target_vel = target_vel;
	stc_previous_feedback_vel = feedback_vel;
	stc_previous_time = current_time;
	stc_previous_feedback = feedback;
	stc_target_pos = target;
	
	// Calculate and return the control value
	return RobotUtil::limit(stc_cntl_min, stc_cntl_max,
	    (stc_const_f * (target_vel / stc_vel_max) ) +
		(stc_const_p * stc_error_p) +
		(stc_const_i * stc_error_i) +
		(stc_const_d * stc_error_d));
}

/*******************************************************************************
 *
 * Set the proportional, integral, and differential constants
 *
 ******************************************************************************/
void SimpleTrapCntl::setControlConstants(float f, float p, float i, float d)
{
	stc_const_f = f;
	stc_const_p = p;
	stc_const_i = i;
	stc_const_d = d;
}

/*******************************************************************************
 *
 * Set the target (and feedback) range
 *
 * @param 	min	minimum target value
 * @param 	max	maximum target value
 *
 * @param	thp	threshold percentage -- reset error accumulators if input
 * 				target changes by this percentage of the total target range
 * 				defaults to 0.20 (20 %)
 *
 ******************************************************************************/
void SimpleTrapCntl::setVelocityLimits(float min, float max, float thp)
{
	stc_vel_min = min;
	stc_vel_max = max;

	stc_tolerance = (max - min) / 200.0; // half percent of velocity range

	stc_threshold = (max - min) * thp;
}

/*******************************************************************************
 *
 * Set the control range and initial value
 *
 ******************************************************************************/
void SimpleTrapCntl::setControlValueLimits(float min, float max)
{
	stc_cntl_min = min;
	stc_cntl_max = max;
}

/*******************************************************************************
 *
 ******************************************************************************/
void SimpleTrapCntl::setAccelerationLimits(float val)
{
	stc_accel = val;
}

/*******************************************************************************
 *
 * Reset PID by clearing the error variables and setting the control value
 * to the initial control value.
 *
 ******************************************************************************/
void SimpleTrapCntl::reset()
{
	stc_error_p = 0.0;
	stc_error_i = 0.0;
	stc_error_d = 0.0;
}

/*******************************************************************************
 *
 * Get the proportional portion of the error adjusted based on Kp
 * 
 ******************************************************************************/
float SimpleTrapCntl::getErrorP()
{
	return (stc_const_p * stc_error_p);
}

/*******************************************************************************
 *
 * Get the integral portion of the error adjusted based on Ki
 * 
 ******************************************************************************/
float SimpleTrapCntl::getErrorI()
{
	return (stc_const_i * stc_error_i);
}

/*******************************************************************************
 *
 * Get the differential portion of the error adjusted based on Kd
 * 
 ******************************************************************************/
float SimpleTrapCntl::getErrorD()
{
	return (stc_const_d * stc_error_d);
}

/*******************************************************************************
 *
 * Getters -- so data can be included in logs
 *
 ******************************************************************************/
float SimpleTrapCntl::getVelocityMin(void) 	{return stc_vel_min;}
float SimpleTrapCntl::getVelocityMax(void) 	{return stc_vel_max;}
float SimpleTrapCntl::getControlMin(void) 	{return stc_cntl_min;}
float SimpleTrapCntl::getControlMax(void) 	{return stc_cntl_max;}
float SimpleTrapCntl::getThreshold(void) 	{return stc_threshold;}
float SimpleTrapCntl::getTargetVel(void) 	{return stc_previous_target_vel;}
float SimpleTrapCntl::getMeasuredVel(void) 	{return stc_previous_feedback_vel;}
float SimpleTrapCntl::getTargetPos(void) 	{return stc_target_pos;}
