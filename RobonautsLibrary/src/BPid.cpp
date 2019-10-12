/*******************************************************************************
 *
 * File: BPid.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/BPid.h"
#include "RobonautsLibrary/RobotUtil.h"

#include <math.h>

/******************************************************************************
 * constructor: initialize variable
 ******************************************************************************/
BPid::BPid()
{
	reset();
}

/******************************************************************************
 * destructor: clean up
 ******************************************************************************/
BPid::~BPid()
{

}

/******************************************************************************
 * reset by re-initializing all variables; (possible never needeed)
 ******************************************************************************/
void BPid::reset()
{
	m_enabled = true;
	m_gains_set = false;
	m_limits_set = false;
	m_is_angle = false;

	m_ki = 0.0;
	m_kd = 0.0;
	m_kp = 0.0;
	m_ref = 0.0;
	m_integrate_threshold = 100000000.0f; // set value "large", so integration not used until set
	m_filter_coeff = 0.0;   // 0.0 implies no filtering

	m_error = 0.0;
	m_ref = 0.0; // initialize setpoint
	m_prev_error = 0.0f; // zero integrator
	m_delta_err = 0.0f; // zero difference
	m_delta_err_filter = 0.0f;  // difference between current and prior m_error (can be low passed)

#if HIGHPASS
	// initialize highpass filter parameters
	last_highpass_output = 0.0f;
	last_highpass_input = 0.0f;
	hp_coeff = 0.0; // high pass filter coefficent; will be enabled if needed
#endif

	m_ffwd_0 = 0.0; // feed forward coefficient (input^3)
	m_ffwd_1 = 0.0; // feed forward coefficient (input^2)
	m_ffwd_2 = 0.0; // feed forward coefficient (input^1)
	m_ffwd_3 = 0.0; // feed forward coefficient (input^0)

	m_actual = 0.0;
	m_mot_cmd = 0.0;
	m_limits[0] = m_limits[1] = 0.0;

}

/******************************************************************************
 * reset just the variables that are persistent
 ******************************************************************************/
void BPid::reset_internal()
{
	m_error = 0.0;
	m_ref = 0.0; // initialize setpoint
	m_prev_error = 0.0f; // zero integrator
	m_delta_err = 0.0f; // zero difference
	m_delta_err_filter = 0.0f;  // difference between current and prior m_error (can be low passed)

}

/******************************************************************************
 *	set the p gain, i gain, threshold for engaging controller
 ******************************************************************************/
void BPid::setGains(double kp, double ki, double kd, double threshold)
{
	m_gains_set = true;
	m_kp = kp;
	m_ki = ki;
	m_kd = kd;
	m_integrate_threshold = threshold;
}

/******************************************************************************
 *	set the p gain, i gain, threshold for engaging controller
 ******************************************************************************/
void BPid::setLimits(double limitLower, double limitUpper)
{
	m_limits_set = true; // if the controller runs without first setting
	m_limits[0] = limitLower;
	m_limits[1] = limitUpper;
}

/******************************************************************************
 *	set the feedforward polynomial gains
 ******************************************************************************/
void BPid::setFeedFowardCoefficient(double a0, double a1, double a2, double a3)
{
	m_ffwd_0 = a0; // feed forward coefficient (input^3)
	m_ffwd_1 = a1; // feed forward coefficient (input^2)
	m_ffwd_2 = a2; // feed forward coefficient (input^1)
	m_ffwd_3 = a3; // feed forward coefficient (input^0)
}

/******************************************************************************
 *	set whether it is an angle or not, which treats errors differently
 ******************************************************************************/
void BPid::setIsAngle(bool is_angle)
{
	m_is_angle = is_angle;
}

/******************************************************************************
 *	disable the control system
 ******************************************************************************/
void BPid::disable()
{
	m_enabled = false;
}

/******************************************************************************
 *	enable the control system
 ******************************************************************************/
void BPid::enable()
{
	m_enabled = true;
}

/******************************************************************************
 *	set enabled state to parameter
 ******************************************************************************/
void BPid::setState(bool enabledA)
{
	m_enabled = enabledA;
}

/******************************************************************************
 *  create feed forward term, fit third order polynomial
 ******************************************************************************/
double BPid::feedForward(double reference)
{
	return (pow(reference, 3) * m_ffwd_0 + pow(reference, 2) * m_ffwd_1 + pow(
	    reference, 1) * m_ffwd_2 + pow(reference, 0) * m_ffwd_3);
}

/******************************************************************************
 *  the control system; must be enabled to return a value
 ******************************************************************************/
double BPid::update(double reference, double actual)
{
#if HIGHPASS
	double highpass;
#endif  // HIGHPASS
	double err;

	m_actual = actual; // necessary only for logging
	m_ref = reference; // necessary only for logging

	err = (m_ref - m_actual); // determine error


    if (m_is_angle == true)   // if an angle, the error between 170 and -170 is -20, not 340
    {
        int count = 0;
        while (std::abs(err) > 180.0)
        {
            err -= std::copysign(1.0, err) * 360.0;
            count++;
            if (count > 100)  // this would be an error of 18000 deg, major malfunction or lots of loops, prevents (unlikely) infinite loop
            {
                err = 0;
                break;
            }
        }
    }

	if (std::fabs(err) < m_integrate_threshold) // accumulate error
	{
		m_prev_error += err; // integrate m_error when small (integrate threshold defines small)
	}
	else
	{
		m_prev_error /= 2.0; // decay error when large
	}

	m_delta_err = err - m_error;
	m_delta_err_filter = RobotUtil::lowPass(m_delta_err, &m_delta_err_filter, m_filter_coeff);

	//*********** begin: gain scheduling *****************
	// non-linear control law to get a fast star with low gains
	// NOTE: gain scheduling will be added if performance warrants
#if GAIN_SCHEDULE
	if(fabs(m_error) > m_integrate_threshold)
	{
		error /= 2.0;
	}
#endif		// GAIN_SCHEDULE
	//*********** end: gain scheduling *****************

	m_mot_cmd = feedForward(m_ref) + m_kp * err + m_ki * m_prev_error + m_kd * m_delta_err;

	m_error = err;

	// NOTE: highpass will be added if performance warrants
	// use highpass filter to get a quick start, then fade out
#if HIGHPASS
	//highpass = RobotUtil::highPass(mot_cmd, &last_highpass_input, &last_highpass_output, hp_coeff);
	//mot_cmd += highpass;
#endif  // HIGHPASS
	m_mot_cmd = RobotUtil::limit(m_limits[0], m_limits[1], m_mot_cmd);

	return m_mot_cmd;
}
