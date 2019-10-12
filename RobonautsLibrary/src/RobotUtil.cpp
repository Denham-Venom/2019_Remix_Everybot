/*******************************************************************************
 *
 * File: RobotUtil.cpp
 * 
 * This file contains the definition of a class that is used to hold utility
 * type methods that as of yet don't fit anywhere else.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#include <math.h>
#include "RobonautsLibrary/RobotUtil.h"
#include <string>

using namespace std;
using namespace frc;

const double RobotUtil::DEGREE_PER_RADIAN = 57.295779513082;

PowerDistributionPanel *RobotUtil::util_power_panel = nullptr;
/*******************************************************************************
 *
 * Splits a string into tokens
 *
 ******************************************************************************/
void RobotUtil::split(vector<string> *tokens, string text, char sep,
    bool include_empty)
{

	int idx = text.find_last_not_of(" \n\r\t");
	text = text.substr(0, idx + 1);

	unsigned int start = 0;
	unsigned int end = 0;
	string tok;

	while ((end = text.find(sep, start)) != string::npos)
	{
		if (((end - start) > 0) || include_empty)
		{
			tokens->push_back(text.substr(start, end - start));
		}
		start = end + 1;
	}

	tokens->push_back(text.substr(start));
}

/*******************************************************************************
 * changes string to upper
 ******************************************************************************/
std::string RobotUtil::toUpper(std::string log_name)
{
	unsigned int i = 0;

	while (i < log_name.length())
	{
		log_name.at(i) = toupper(log_name.at(i));
		i++;
	}

	return log_name;
}

/*******************************************************************************
 *
 * Limit the provided value between the provided limits.
 *
 ******************************************************************************/
double RobotUtil::limit(double lim1, double lim2, double val)
{
    if (lim1 < lim2)
    {
        if (val < lim1)
        {
            return lim1;
        }

        if (val > lim2)
        {
            return lim2;
        }

        return val;
    }
    else
    {
        if (val < lim2)
        {
            return lim2;
        }

        if (val > lim1)
        {
            return lim1;
        }

        return val;
    }
}

/*******************************************************************************
 *
 * the returned value is limited both by position constraints and power
 * constraints -- if the position is already outside of the specified range,
 * it can not be moved any further in that direction.
 *
 ******************************************************************************/
double RobotUtil::directionalLimit(double min_pos, double max_pos, double pos,
    double min_pwr, double max_pwr, double pwr)
{
	if (pos > max_pos)
	{
		return RobotUtil::limit(0.0, max_pwr, pwr);
	}
	else if (pos < min_pos)
	{
		return RobotUtil::limit(min_pwr, 0.0, pwr);
	}
	else
	{
		return RobotUtil::limit(min_pwr, max_pwr, pwr);
	}
}

/*******************************************************************************
 *
 * rate limit; steps per cycle
 * 
 * @param	max		the maximum amount by which the value can change
 * @param	val		the input value
 * @param	last	the previous input value
 * 
 * @return	the last value adjusted by the maximum allowed adjustment or the
 * 			input value if it can be reached by adjusting the last value by
 * 			an amount less that the maximum allowed change.
 *
 ******************************************************************************/
double RobotUtil::rateLimit(double max, double val, double last)
{
	// from excel =IF(ABS(B3-C2)>=E$2, C2+SIGN(B3-C2)*E$2, B3)
	double delta = val - last;
	double signDelta = RobotUtil::sign(delta);
	double retL = val;

	if (fabs(delta) >= max)
	{
		retL = last + signDelta * max;
	}

	return retL;
}

/*******************************************************************************
 *
 *  Get the sign of the specified value.
 *
 *  @return 1.0 if the value is positive, 0.0 if the value is 0.0 and -1.0 if 
 *          the value is negative.
 * 
 ******************************************************************************/
double RobotUtil::sign(double val)
{
	double retL = 0.0;
	if (val != 0.0)
	{
		retL = val / fabs(val);
	}
	return (retL);
}

/*******************************************************************************
 * 
 * Runs a first order low pass filter using externally held information
 * 
 * @param	newValue    the new input value
 * @param	lastValue	the previous value, will be updated by this method
 * @param	filterCoeff	the filter coefficient
 * 
 * @return	the specified value filtered by the coefficient
 * 
 ******************************************************************************/
double RobotUtil::lowPass(double newValue, double *lastValue, double filterCoeff)
{
	double retL;

	retL = filterCoeff * (*lastValue) + (1 - filterCoeff) * newValue;
	*lastValue = retL;

	return (retL);

}

/*******************************************************************************
* 
 * Runs a first order high pass filter using externally held information
 * 
 * @param	newValue    the new input value
 * @param	lastInput	the previous value, will be updated by this method
 * @param	lastOutput	the previous value, will be updated by this method
 * @param	filterCoeff	the filter coefficient
 * 
 * @return	the specified value filtered by the coefficient
 * 
 ******************************************************************************/
double RobotUtil::highPass(double newValue, double *lastInput, double *lastOutput,
    double filterCoeff)
{
	double retL;

	retL = filterCoeff * ((*lastOutput) + newValue - (*lastInput));
	*lastOutput = retL;
	*lastInput = newValue;

	return (retL);
}

/*******************************************************************************
 * 
 * Joystick deadband, symmetric, double
 * 
 * @param	input	the new joystick value
 * @param	db		the deadband width
 * @param	max		the maximum value the joystick will indicate
 * 
 * @return	if input is between -db and db, a value of 0.0 will be returned
 * 			otherwise a value on the slope line produced by connecting 
 *          -max,-max to -db,0.0 or db, 0.0 to max,max
 * 
 ******************************************************************************/
double RobotUtil::deadbandJoy(double input, double db, double max)
{
	double output;

	input = RobotUtil::limit(-max, max, input); // force input to make sure it's within max
	if (fabs(max - db) < 0.001)
		return input;

	double scale_intercept = db / (max - db);
	double scale_slope = max / (max - db);

	if (input < (-1 * db))
	{
		output = (input * scale_slope) + scale_intercept;
	}
	else if (input > db)
	{
		output = (input * scale_slope) - scale_intercept;
	}
	else
	{
		output = 0.0;
	}

	return (output);
}

bool RobotUtil::increasingInMagnitude(double input, double last_input)
{
	bool retL = false;

	if((input > last_input && input > 0) || (input < last_input && input < 0) )
	{
		retL = true;
	}
	return(retL);
}

bool RobotUtil::steady(double input, double last_input)
{
	bool retL = false;

	if(input == last_input)
	{
		retL = true;
	}
	return(retL);
}

double RobotUtil::getCurrent(uint8_t channel)
{
	if (util_power_panel == nullptr)
	{
		// NOTE: attempting to initialize this as a static instance
		//       causes a segmentation fault. Thus, it is initialized
		//       on demand.
		util_power_panel = new PowerDistributionPanel();
	}

	return util_power_panel->GetCurrent(channel);
}


//***************************************************************************
// Below RobotUtil functions for doing basic 2D transformation math
//***************************************************************************

// get minimum distance between two angle, managing across +180/-180 barrier
double RobotUtil::getMinAngleError(double angle1, double angle2)
{
	int count = 0;
	double angle_error = angle1 - angle2;

	while (std::abs(angle_error) > 180.0)  // unwind angle errors > 180 - like when new_angle = -179.9 and desired_angle = 180
	{
		angle_error -= RobotUtil::sign(angle_error) * 360;
		count++;
		if (count > 100)  // this would be an error of 18000 deg, major malfunction or lots of loops, prevents (unlikely) infinite loop
		{
			angle_error = 0.0;
			break;
		}
	}
	return (angle_error);
}

void RobotUtil::TransformationMatrix2D(RobotUtil::Transform2D *T, double angle_rad, RobotUtil::Pnt p)
{	
	TransformationMatrix2D(T, angle_rad, p.x, p.y);

}

void RobotUtil::TransformationMatrix2D(RobotUtil::Transform2D *trans, double angle_rad, double x, double y)
{	
    trans->T[0][0] = trans->T[1][1] = std::cos(angle_rad);
	trans->T[0][1] = -std::sin(angle_rad);
    trans->T[1][0] = -trans->T[0][1];
    trans->T[0][2] = x;
    trans->T[1][2] = y;
    trans->T[2][0] = trans->T[2][1] = 0.0;
    trans->T[2][2] = 1.0;

}
// multiple a 2D transformation by a point, transforming the coordinates of the points into the T coordinates
void RobotUtil::InvertTransformationMatrix2D(RobotUtil::Transform2D *Tinv, RobotUtil::Transform2D *T)
{
    Tinv->T[0][0] = Tinv->T[1][1] = T->T[0][0];
    Tinv->T[0][1] = -T->T[0][1];
    Tinv->T[1][0] = -T->T[1][0];

    Tinv->T[0][2] = -(Tinv->T[0][0] * T->T[0][2] + Tinv->T[0][1] * T->T[1][2]);
    Tinv->T[1][2] = -(Tinv->T[1][0] * T->T[0][2] + Tinv->T[1][1] * T->T[1][2]);
    Tinv->T[2][0] = Tinv->T[2][1] = 0.0;
    Tinv->T[2][2] = 1.0;

}

// multiple a 2D transformation by a point, transforming the coordinates of the points into the T coordinates
void RobotUtil::TransformationMult(RobotUtil::Pnt *pt, RobotUtil::Transform2D *T, RobotUtil::Pnt *p)
{
	pt->x = T->T[0][0] * p->x + T->T[0][1] * p->y + T->T[0][2];
	pt->y = T->T[1][0] * p->x + T->T[1][1] * p->y + T->T[1][2];
}

double RobotUtil::CartesianDistance(double x1, double y1, double x2, double y2)
{
	return (std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}


//**********************************************************
// washout a signal;
//**********************************************************
WashoutCommand::WashoutCommand()
{
	initialized = false;
	last_input = 0.0;
	last_decay = 0.0;

	setDecayPercent(1.0);
	setCoefficient(1.0);
}

WashoutCommand::WashoutCommand(double coefficient, double decay_perc)
{
	initialized = false;
	last_input = 0.0;
	last_decay = 0.0;

	setDecayPercent(decay_perc);
	setCoefficient(coefficient);
}

WashoutCommand::~WashoutCommand()
{

}

double WashoutCommand::setCoefficient(double coefficient)
{
	coeff = coefficient;
	return(coeff);
}

double WashoutCommand::setDecayPercent(double decay_perc)
{
	decay_percent = decay_perc;
	return(decay_percent);
}

double WashoutCommand::update(double input)
{
	double increasing_or_steady;
	double steady_state;
	double decay = .5f;

	value = input;

	if(initialized == false)  // inititalize class when first classed
	{
		initialized = true;
	}
	else
	{
		increasing_or_steady = RobotUtil::increasingInMagnitude(input, last_input) || RobotUtil::steady(input, last_input);
		steady_state = input * decay_percent;
		decay = increasing_or_steady * (coeff * (input - last_input + last_decay));
		value = decay + steady_state;
		if(((value > input) && (input > 0)) || ((value < input) && (input < 0)))
		{
			value = input;
		}
//		Advisory::pinfo("ios %.0f in %.3f dec %.3f ss %.3f val = %.3f coef %.3f  dp %.3f ",
//				increasing_or_steady, input, decay, steady_state, value, coeff, decay_percent);
	}
	last_input = input;
	last_decay = decay;
	return(value);
}


//*************************************************************************
// Very Base Trapezoidal profile generator
// Initial and final velocity are zero, not maximum acceleration
//*************************************************************************
TrapezoidalProfile::TrapezoidalProfile()
{
	max_velocity = fabs(1.0f);
	percent_acc = fabs(0.2f);
	dt = fabs(0.02f);

	acc = last_acc = 0.0;
	vel = last_vel = 0.0;
	pos = last_pos = 0.0;

	target_time = 0.0;
	target_pos = 0.0;
	time_1 = time_2 = 0.0;
	max_acc = 0.0;
	running_time = 0.0;
	initialized = false;
}

TrapezoidalProfile::TrapezoidalProfile(double max_vel, double perc_acc, double delta_t)
{
	max_velocity = fabs(max_vel);
	percent_acc = fabs(perc_acc);
	dt = fabs(delta_t);

	acc = last_acc = 0.0;
	vel = last_vel = 0.0;
	pos = last_pos = 0.0;

	target_time = 0.0;
	target_pos = 0.0;
	time_1 = time_2 = 0.0;
	max_acc = 0.0;
	running_time = 0.0;
	initialized = false;
}

TrapezoidalProfile::~TrapezoidalProfile()
{

}

double TrapezoidalProfile::setMaxVelocity(double value)
{
	max_velocity = fabs(value);
	return(max_velocity);

}

double TrapezoidalProfile::setPercentAcc(double value)
{
	percent_acc = fabs(value);
	return(percent_acc);

}

double TrapezoidalProfile::setDeltaTime(double value)
{
	dt = fabs(value);
	return(dt);

}

double TrapezoidalProfile::getPos()
{
	return(pos);
}

double TrapezoidalProfile::getVel()
{
	return(vel);

}

double TrapezoidalProfile::getAcc()
{
	return(acc);

}

bool TrapezoidalProfile::update()
{
	if(initialized == true)
	{
		// use euler integration (not absolutely accurate; close enough for this if loops running about 50 hz and rates up to 100 units/sec
		running_time += dt;
		if(running_time > target_time)
		{
			acc = 0.0;
			pos = target_pos;
			vel = 0;
			initialized = false;  // trajector is complete
		}
		else if(running_time <= time_1)
		{
			acc = max_acc;
		}
		else if(running_time < time_2)
		{
			acc = 0.0;
			vel = max_velocity * max_acc / fabs(max_acc);
		}
		else if(running_time >= time_2)
		{
			acc = -max_acc;
		}
		// most simple of integrations
		vel += dt * acc;
		pos += dt * vel;
	}
#if 0
	if(m_initialized == true)
	{
		Advisory::pinfo("TrapezoidalProfile::update(%%=%.3f p=%.3f  r=%.3f mv=%.3f, ma=%.3f, t=%.3f t1=%.3f t2=%.3f te=%.3f)",
				percent_acc, m_pos, m_target_pos, m_ave_vel, max_acc, m_running_time, m_time_1, m_time_2, m_target_time);
	}
#endif
	
	return(initialized);
}

bool TrapezoidalProfile::initialize(double current, double end)
{
	initialized = false;
	target_pos = end;
	if(max_velocity > 0 && !(percent_acc > 1.0) && percent_acc != 0.0)    // avoid divide by zero errors
	{
		target_time  = (end - current)/(max_velocity * (1-percent_acc));
		time_1 = percent_acc * target_time;
		time_2 = target_time - time_1;
		acc = 0.0;
		vel = 0.0;
		pos = current;
		if(time_1 != 0.0)
		{
			running_time = 0.0;
			max_acc = max_velocity / time_1;
			initialized = true;
			target_time = fabs(target_time);
			time_1 = fabs(time_1);
			time_2 = fabs(time_2);
		}
		else 
		{
		}
	}
//	Advisory::pinfo("TrapezoidalProfile::initialize(%.3f  %.3f %.3f)", current, end, m_target_pos);
	return(initialized);
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


//*************************************************************************
// A just beyond basic Trapezoidal profile generator
// Initial and final velocity are zero
// Can have asymmetric accel and decel profile
// runs a sim of the trajectory then adjusts the position to handle
//     quantization and round off error
//*************************************************************************
TrapezoidProfile2::TrapezoidProfile2()
{
	m_percent_acc = 0.02f;
	m_percent_decel = 0.02f;
	m_dt = 0.02f;

	m_acc = m_last_acc = 0.0;
	m_vel = m_last_vel = 0.0;
	m_pos = m_last_pos = 0.0;
	m_ave_vel = 0.0;
	m_max_vel = 0.0;
	m_adjustment = 0.0;
	m_target_time = 0.0;
	m_target_pos = 0.0;
	m_time_1 = m_time_2 = 0.0;
	max_acc = 0.0;
	m_running_time = 0.0;
	m_initialized = false;
	m_init_pos = 0.0;

	TrapezoidProfile2(0.0f, 0.2f, 0.02f);
}

TrapezoidProfile2::TrapezoidProfile2(double acc_percent, double decel_percent, double delta_t)
{

	m_percent_acc = fabs(acc_percent);
	m_percent_decel = fabs(decel_percent);
	m_dt = fabs(delta_t);

	m_acc = m_last_acc = 0.0;
	m_vel = m_last_vel = 0.0;
	m_pos = m_last_pos = 0.0;

	m_ave_vel = 0.0;
	m_max_vel = 0.0;
	m_dt = 0.02f;
	m_adjustment = 0.0;
	m_target_time = 0.0;
	m_target_pos = 0.0;
	m_time_1 = m_time_2 = 0.0;
	max_acc = 0.0;
	m_running_time = 0.0;
	m_initialized = false;
	m_init_pos = 0.0;
}

TrapezoidProfile2::~TrapezoidProfile2()
{

}

void TrapezoidProfile2::classInit(double acc_percent, double decel_percent, double delta_t)
{
	m_percent_acc = fabs(acc_percent);
	m_percent_decel = fabs(decel_percent);
	m_dt = fabs(delta_t);

	m_acc = m_last_acc = 0.0;
	m_vel = m_last_vel = 0.0;
	m_pos = m_last_pos = 0.0;

	m_max_vel = 0.0;
	m_adjustment = 1.0;
	m_target_time = 0.0;
	m_target_pos = 0.0;
	m_time_1 = m_time_2 = 0.0;
	max_acc = 0.0;
	m_running_time = 0.0;
	m_initialized = false;

	m_init_pos = 0.0;
}

void TrapezoidProfile2::setPercentAcc(double acc, double decel)
{
	m_percent_acc = fabs(acc);
	m_percent_decel = fabs(decel);
}

void TrapezoidProfile2::setDeltaTime(double value)
{
	m_dt = fabs(value);
}


void TrapezoidProfile2::setPos(double pos)
{
	m_initialized = false;
	m_adjustment = 1.0;
	m_pos = pos;
}

double TrapezoidProfile2::getPos()
{
	return(m_pos*m_adjustment);
}

double TrapezoidProfile2::getVel()
{
	return(m_vel);

}

double TrapezoidProfile2::getAcc()
{
	return(m_acc);

}

bool TrapezoidProfile2::update()
{
	double vel_prior = m_vel;
	if(m_initialized == true)
	{
		// use euler integration (not absolutely accurate; close enough for this if loops running about 50 hz and rates up to 100 units/sec
		m_vel = calcVel(m_running_time, m_max_vel, m_time_1, m_time_2, m_target_time);

		m_pos += m_dt * m_vel;
		m_acc = (m_vel - vel_prior) / m_dt;
		m_running_time += m_dt;
	}
#if 0
	if(m_initialized == true)
	{
		Advisory::pinfo("TrapezoidalProfile::update(%%=%.3f p=%.3f  r=%.3f mv=%.3f, ma=%.3f, t=%.3f t1=%.3f t2=%.3f te=%.3f)",
				percent_acc, m_pos, m_target_pos, m_ave_vel, max_acc, m_running_time, m_time_1, m_time_2, m_target_time);
	}
#endif

	return(m_initialized);
}

bool TrapezoidProfile2::initialize(double end, double ave_vel, double start)
{
	m_initialized = false;
	m_target_pos = end;
	m_init_pos = start;
	m_ave_vel = fabs(ave_vel);
	if(m_ave_vel != 0  && m_dt > 0.0)    // avoid divide by zero errors
	{
		m_target_time  = fabs((end - start)/m_ave_vel);
		m_time_1 = m_percent_acc * m_target_time;
		m_time_2 = (1 - m_percent_decel) *m_target_time;
		m_acc = 0.0;
		m_vel = 0.0;
		m_pos = start;
		m_running_time = 0.0;
		m_initialized = true;
		m_time_1 = fabs(m_time_1);
		m_time_2 = fabs(m_time_2);

		m_max_vel = (m_target_pos - start) / (0.5f * m_target_time * (m_percent_acc + m_percent_decel + 2.0f * (1 - m_percent_acc - m_percent_decel)));

		for(int i=0;i<int(0.5 + m_target_time/m_dt);i++)
		{
			m_vel = calcVel(m_running_time, m_max_vel, m_time_1, m_time_2, m_target_time);
			m_pos += m_dt * m_vel;

			m_running_time += m_dt;
		}
		m_adjustment = m_target_pos / m_pos;
		m_acc = 0.0;
		m_vel = 0.0;
		m_pos = start;
		m_running_time = 0.0;
	}
	return(m_initialized);
}

void TrapezoidProfile2::disable(double position)
{
	m_initialized = false;
	m_acc = 0.0;
	m_vel = 0.0;
	m_pos = position;
}

double TrapezoidProfile2::calcVel(double time_in, double max_vel, double time_1, double time_2, double end_time)
{
	double vel=0.0;
	if(time_in < time_1)  // accelerating
	{
		vel = time_in * max_vel / time_1;
	}
	else if(time_in < time_2)  // constant velocity
	{
		vel = max_vel;
	}
	else if(time_in <= end_time)  // decelerating
	{
		vel = max_vel - (time_in - time_2) * max_vel / (end_time - time_2);
	}
	else
	{
		vel = 0.0;
	}
	return(vel);

}


/*******************************************************************************
 *******************************************************************************
 *******************************************************************************
 *
 *  TrapezoidProfile3
 *
 *******************************************************************************
 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *
 ******************************************************************************/
TrapezoidProfile3::TrapezoidProfile3(void)
{
    m_target_position = 0.0;
    m_desired_position = 0.0;
    m_desired_velocity = 0.0;

    m_max_velocity = 0.0;
    m_desired_decceleration = 0.0;
    m_desired_acceleration = 0.0;
    m_position_tolerance = 0.0;
    m_control_period = 0.1;
}

/*******************************************************************************
 *
 ******************************************************************************/
TrapezoidProfile3::~TrapezoidProfile3(void)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void TrapezoidProfile3::setConfiguration(double max_velocity, double desired_acceleration, double desired_deceleration, double control_period)
{
     m_max_velocity = max_velocity;
     m_desired_acceleration = desired_acceleration;
     m_desired_decceleration = desired_deceleration;
     m_control_period = control_period;

     //Initialize tolerance to 20% of max distance moved in one period
     m_position_tolerance = m_control_period * m_max_velocity * 0.20;

//    Advisory::pinfo("setConfiguration(%f, %f, %f, %f\n", max_velocity,
//         desired_acceleration, desired_deceleration, control_period );
}

/*******************************************************************************
 *
 ******************************************************************************/
void TrapezoidProfile3::setInitialPosition(double position)
{
    m_target_position = position;
    m_desired_position = position;

    m_desired_velocity = 0.0;
}

/*******************************************************************************
 *
 ******************************************************************************/
void TrapezoidProfile3::setTargetPosition(double new_target)
{
    m_target_position = new_target;
}

/*******************************************************************************
 *
 * Update the trajectory path for the next cycle
 *
 * @return the new trajectory path position
 *
 ******************************************************************************/
double TrapezoidProfile3::update()
{
    double sign = (m_target_position >= m_desired_position)?1.0:-1.0;
    double distance_to_go = fabs(m_target_position - m_desired_position);

 //   Advisory::pinfo("trap tp=%f dp=%f, dv=%f, maxv=%f, acc=%f", m_target_position, m_desired_position, m_desired_velocity, m_max_velocity, m_desired_acceleration);
	if (distance_to_go < m_position_tolerance)
    {
        m_desired_position = m_target_position;
        m_desired_velocity = 0.0;

        return m_desired_position;
    }

    // Use the kinematic equation to find the initial velocity
    // that would result in a final velocity of zero at the target
    // vf**2 = vi**2 + 2 * decel * dist_to_go
    // --> vi = sqrt(2 * decel  * dist_to_go)
    double desired_velocity = sign * sqrt(2 * distance_to_go * m_desired_decceleration);

    // limit the desired velocity between +/- max velocity
    if (desired_velocity > m_max_velocity)
    {
        desired_velocity = m_max_velocity;
    }
    else if (desired_velocity < -1.0 * m_max_velocity)
    {
        desired_velocity = -1.0 * m_max_velocity;
    }

    // if we are still accelerating, further limit the desired velocity
    // based on acceleration rate

    if (sign > 0.0)
    {
        if (m_desired_velocity + (m_desired_acceleration* m_control_period) < desired_velocity)
        {
            desired_velocity = m_desired_velocity + (m_desired_acceleration* m_control_period);
        }
     }
    else
    {
        if (m_desired_velocity - (m_desired_acceleration* m_control_period) > desired_velocity)
        {
            desired_velocity = m_desired_velocity - (m_desired_acceleration* m_control_period);
        }
    }

    // Calculate the new position, remember the new position and velocity for the next update
    m_desired_position = (desired_velocity * m_control_period) + m_desired_position;
    m_desired_velocity = desired_velocity;

   return m_desired_position;
}

/*******************************************************************************
 *
 ******************************************************************************/
double TrapezoidProfile3::getTrajectoryPosition(void)
{
    return m_desired_position;
}

/*******************************************************************************
 *
 ******************************************************************************/
double TrapezoidProfile3::getTrajectoryVelocity(void)
{
    return m_desired_velocity;
}
