/*******************************************************************************
 *
 * File: SimplePID.cpp
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

#include "RobonautsLibrary/SimplePID.h"
#include "gsu/Advisory.h"
#include "RobonautsLibrary/RobotUtil.h"

/*******************************************************************************
 *
 * Allocate a PID object and give all instance variables default values.
 *
 ******************************************************************************/
SimplePID::SimplePID(void)
{
	// make sure everything gets initialized to something safe
	setPIDConstants(1.0, 0.0, 0.0);
	setControlLimits(-1.0, 1.0);
	setTargetLimits(-1.0, 1.0);
	spid_prev_targ = 0.0;

	reset();
}

/*******************************************************************************
 *
 * Release all resources used by this object.
 *
 ******************************************************************************/
SimplePID::~SimplePID(void)
{
}

/*******************************************************************************
 *
 * Calculate the control (manipulated) variable
 *
 ******************************************************************************/
float SimplePID::calculateControlValue(float target, float feedback)
{
	target = RobotUtil::limit(spid_targ_min, spid_targ_max, target);

	//
	// calculate the errors
	//
	float error_cur = target - feedback;

	if (error_cur * spid_error_i < 0.0)  // the sign of i != the sign of error_cur
//	if (std::fabs((spid_prev_targ - target) / (spid_targ_max - spid_targ_min)) > spid_threshold)
	{
		spid_error_i = 0.0;
		spid_error_d = 0.0;
	}
	else
	{
		spid_error_i += error_cur;

		// limiting this accumulator reduces overshoot significantly
		// limiting it in this way prevents divide by zero (as long
		// as control_max is positive and control_min is negative)
		if ((spid_const_i * spid_error_i) > spid_cntl_max)
		{
			spid_error_i = spid_cntl_max / spid_const_i;
		}
		else if ((spid_const_i * spid_error_i) < spid_cntl_min)
		{
			spid_error_i = spid_cntl_min / spid_const_i;
		}

		spid_error_d = (error_cur - spid_error_p);
	}
	spid_error_p = error_cur;

	spid_prev_targ = target;

	// Calculate and return the control value
#if 0
	float ep = stc_const_p * spid_error_p;
	float ei = spid_const_i * spid_error_i;
	float ed = spid_const_d * spid_error_d;
	float et = ep + ei + ed;
	float el = RobotUtil::limit(spid_cntl_min, spid_cntl_max, et);
	
	Advisory::pinfo("%s:: T: v=%f, f=%f, mn=%f, mx=%f, C: mn=%f, mx=%f, E: p=%f, i=%f, d=%f, t=%f, lim=%f   ",
			spid_param_prefix.c_str(), target, feedback, spid_targ_min, 
			spid_targ_max, spid_cntl_min, spid_cntl_max, ep, ei, ed, et, el);
	return el;
#else
	return RobotUtil::limit(spid_cntl_min, spid_cntl_max,
	    (stc_const_p * spid_error_p) + (spid_const_i * spid_error_i)
	        + (spid_const_d * spid_error_d));
#endif
}

/*******************************************************************************
 *
 * Set the proportional, integral, and differential constants
 *
 ******************************************************************************/
void SimplePID::setPIDConstants(float p, float i, float d)
{
	stc_const_p = p;
	spid_const_i = i;
	spid_const_d = d;
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
void SimplePID::setTargetLimits(float min, float max, float thp)
{
	spid_targ_min = min;
	spid_targ_max = max;

	spid_threshold = (max - min) * thp;
}

/*******************************************************************************
 *
 * Set the control range and initial value
 *
 ******************************************************************************/
void SimplePID::setControlLimits(float min, float max)
{
	spid_cntl_min = min;
	spid_cntl_max = max;
}

/*******************************************************************************
 *
 * Reset PID by clearing the error variables and setting the control value
 * to the initial control value.
 *
 ******************************************************************************/
void SimplePID::reset()
{
	spid_error_p = 0.0;
	spid_error_i = 0.0;
	spid_error_d = 0.0;
}

/*******************************************************************************
 *
 * Get the proportional portion of the error adjusted based on Kp
 * 
 ******************************************************************************/
float SimplePID::getErrorP()
{
	return (stc_const_p * spid_error_p);
}

/*******************************************************************************
 *
 * Get the integral portion of the error adjusted based on Ki
 * 
 ******************************************************************************/
float SimplePID::getErrorI()
{
	return (spid_const_i * spid_error_i);
}

/*******************************************************************************
 *
 * Get the differential portion of the error adjusted based on Kd
 * 
 ******************************************************************************/
float SimplePID::getErrorD()
{
	return (spid_const_d * spid_error_d);
}

/*******************************************************************************
 *
 * Get the proportional gain
 * 
 ******************************************************************************/
float SimplePID::getKp()
{
	return (stc_const_p );
}

/*******************************************************************************
 *
 * Get the integratal gain
 * 
 ******************************************************************************/
float SimplePID::getKi()
{
	return (spid_const_i);
}

/*******************************************************************************
 *
 * Get the derivative gain
 * 
 ******************************************************************************/
float SimplePID::getKd()
{
	return (spid_const_d);
}



/*******************************************************************************
 *
 * Limit the specified value to a value between the minimum and maximum target
 * value.
 *
 ******************************************************************************/
float SimplePID::limitTarget(float val)
{
	return RobotUtil::limit(spid_targ_min, spid_targ_max, val);
}

/*******************************************************************************
 *
 * Limit the specified value to a value between the minimum and maximum control
 * value.
 *
 ******************************************************************************/
float SimplePID::limitControl(float val)
{
	return RobotUtil::limit(spid_cntl_min, spid_cntl_max, val);	
}

/*******************************************************************************
 *
 * Getters -- so data can be included in logs
 *
 ******************************************************************************/
float SimplePID::getTargetMin(void) {return spid_targ_min;}
float SimplePID::getTargetMax(void) {return spid_targ_max;}
float SimplePID::getControlMin(void) {return spid_cntl_min;}
float SimplePID::getControlMax(void) {return spid_cntl_max;}
float SimplePID::getThreshold(void) {return spid_threshold;}

/*******************************************************************************
 *
 * Update the PID constants using the Parameter class and prefix.  
 * 
 * NOTE: This method is not thread protected, thread protection should
 *       be handled in the class using the SimplePID object.
 *
 ******************************************************************************/
void SimplePID::updateConfig(void)
{
//	stc_const_p = Parameter::getAsFloat(spid_param_prefix +"_KP", stc_const_p);
//	spid_const_i = Parameter::getAsFloat(spid_param_prefix +"_KI", spid_const_i);
//	spid_const_d = Parameter::getAsFloat(spid_param_prefix +"_KD", spid_const_d);
//
//	spid_targ_max = Parameter::getAsFloat(spid_param_prefix +"_TMX", spid_targ_max);
//	spid_targ_min = Parameter::getAsFloat(spid_param_prefix +"_TMN", spid_targ_min);
//
//	spid_cntl_max = Parameter::getAsFloat(spid_param_prefix +"_CMX", spid_cntl_max);
//	spid_cntl_min = Parameter::getAsFloat(spid_param_prefix +"_CMN", spid_cntl_min);
//
//	spid_threshold = Parameter::getAsFloat(spid_param_prefix +"_THP", spid_threshold);
}

/*******************************************************************************
 *
 * Set the prifix for Parmaeters used by this class.  The internal "constants"
 * that can be update via the Parameter utility are updated in the 
 * updateConfig method by calling 
 *   const = Parameter::getAsFloat(prefix(prefix + suffix, const);
 *   
 * Where prefix is the value passed to this method and suffix is one of:
 * _KP, _KI, _KD, _TMX, _TMN, _CMX, _CMN, and _THP 
 * 
 * @param prefix	a string prefix, this string will be converted to upper
 * 					case before it is used.
 *
 ******************************************************************************/
void SimplePID::setParamPrefix(std::string prefix)
{
	spid_param_prefix = RobotUtil::toUpper(prefix);	
}
