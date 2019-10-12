/*******************************************************************************
 *
 * File: RGyro.cpp
 *
 * WARNING: This class expects either the Calibrate method or the GetAngle 
 * 			method to be called every period.  Calibrate should be called when
 * 			the robot is not moving (disabled)
 *  
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <cmath>

#include "gsu/Advisory.h"
//#include "frc/WPILib.h"

#include "RobonautsLibrary/RGyro.h"

using namespace frc;

/*******************************************************************************
 *
 * Create an instance of the RGyro class on the default channel and module.
 * 
 ******************************************************************************/
RGyro::RGyro(void) :
	AnalogGyro(1)
{
	drift_rate = 0.0;
	init();
}

/*******************************************************************************
 *
 * Create an instance of the RGyro class on the specified channel and default 
 * module.  Note: because of CRIO hardware, the Gyro must be connected to
 * module 1, channel 1,  but Gyro provides this constructor so it's passed on.
 * 
 * @param	channel	the channel to which the Gyro is connected
 * 
 ******************************************************************************/
RGyro::RGyro(uint32_t channel) :
	AnalogGyro(channel)
{
	drift_rate = 0.0;
	init();
}

/*******************************************************************************
 *
 * Create an instance of the RGyro class on the specified channel and 
 * module.  Note: because of CRIO hardware, the Gyro must be connected to
 * module 1, channel 1,  but Gyro provides this constructor so it's passed on.
 * 
 * @param	module	the module to which the Gyro is connected
 * @param	channel	the channel to which the Gyro is connected
 * 
 ******************************************************************************/
RGyro::RGyro(uint32_t module, uint32_t channel) :
	AnalogGyro(channel)
//	Gyro(module, channel)
{
	drift_rate = 0.0;
	init();
}

/*******************************************************************************
 *
 * Release any resources allocated by this class.
 * 
 ******************************************************************************/
RGyro::~RGyro(void)
{

}

/*******************************************************************************
 *
 * This private initialization method is called from with each constructor and
 * from the Reset method.  It resets the Gyro and checks to make sure it
 * got reset.  If the anything goes wrong in this method the gyro is set as
 * not ready to use.
 * 
 ******************************************************************************/
void RGyro::init()
{
	//
	// reset the gyro, 
	// give it some time to finish the reset,
	// check to make sure the reset worked
	//
	is_ready = false;
	Advisory::pinfo("Before Analog Gyro Reset ");
	AnalogGyro::Reset();
	Advisory::pinfo("After Analog Gyro Reset ");
	offset = 0.0;

	int count = 0;
	while ((fabs(AnalogGyro::GetAngle()) > 1.0) && count < 20)
	{
		count++;
		Wait(0.01);
	}

	// Calibrate in AnalogGyro has a built in 5 second wait time; temporarily comment out
	//Calibrate();
	if (count < 20)
	{
		offset = 0.0;
		is_ready = true;
	}
	cur_raw_angle = 0.0;
	prev_raw_angle = 0.0;
}

/*******************************************************************************
 *
 * @return true if the gyro initialized correctly and no problems were detected
 * 			during normal use.
 * 			
 ******************************************************************************/
bool RGyro::IsReady()
{
	return is_ready;
}

/*******************************************************************************
 *
 * @return the current gyro angle adjusted for the calibrated drift rate
 * 
 ******************************************************************************/
float RGyro::GetAngle()
{
	if (is_ready)
	{
		offset += drift_rate;
		cur_raw_angle = AnalogGyro::GetAngle();
		prev_raw_angle = cur_raw_angle;
	}

	return cur_raw_angle - offset;
}

/*******************************************************************************
 *
 * Calculate a new drift rate and offest based the assumption that the 
 * gyro is not moving.  If the gyro is moving the offset and drift rate will
 * get off.  Assuimg the gyro sits stationary for a  few seconds before the
 * angles are used, the drift rate will settle back to an approriate value.
 * A Reset() before starting will reset the Gyro and offset, a Reset() will not
 * impact the drift rate.
 * 
 * @return the current gyro angle adjusted for the calibrated drift rate
 * 
 ******************************************************************************/
float RGyro::UpdateDrift()
{
	if (is_ready)
	{
		offset += drift_rate;

		cur_raw_angle = AnalogGyro::GetAngle();
		drift_rate = ((drift_rate * 99.0) + (cur_raw_angle - prev_raw_angle))
		    / 100.0;
		prev_raw_angle = cur_raw_angle;
	}

	return cur_raw_angle - offset;
}

/*******************************************************************************
 *
 * Reset the gyro and the calibrated offset.  Also does a check to make sure
 * the gyro has not been unplugged or broken.
 * 
 ******************************************************************************/
void RGyro::Reset()
{
	init();
}

