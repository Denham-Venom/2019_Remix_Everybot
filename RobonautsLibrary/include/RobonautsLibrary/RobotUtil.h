/*******************************************************************************
 *
 * File: RobotUtil.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <string>
#include <vector>

#ifdef _2018
#include <PowerDistributionPanel.h>  // WPI
#else
#include <frc/PowerDistributionPanel.h>  // WPI
#endif

#include "gsu/Advisory.h"
using namespace frc;


/*******************************************************************************
 *
 * This class is used to hold utility type methods that as of yet don't fit 
 * anywhere else.
 *
 ******************************************************************************/
class RobotUtil
{
	public:
		static const double DEGREE_PER_RADIAN;

		static void split(std::vector<std::string> *tokens,
		    const std::string text, char sep, bool include_empty);

		static double limit(double min, double max, double val);

		static double directionalLimit(double min_pos, double max_pos, double pos,
		    double min_pwr, double max_pwr, double pwr);

		static double rateLimit(double max, double val, double last);
		static double sign(double val);
		static double lowPass(double newValue, double *lastValue,
		    double filterCoeff);

		static double highPass(double newValue, double *lastInput,
		    double *lastOutput, double filterCoeff);

		static double deadbandJoy(double input, double db, double max);
		
		static std::string toUpper(std::string log_name);
		
		static bool increasingInMagnitude(double input, double last_input);
		static bool steady(double input, double last_input);
		
		static double getCurrent(uint8_t channel);

		// Simple 2D transformation math

		struct Transform2D 
		{
			double T[3][3];
		};

		struct Pnt
		{
			double x, y;
		};

        static void TransformationMatrix2D(RobotUtil::Transform2D *T, double angle_rad, RobotUtil::Pnt p);
        static void TransformationMatrix2D(RobotUtil::Transform2D *T, double angle_rad, double x, double y);
        static void InvertTransformationMatrix2D(RobotUtil::Transform2D *Tinv, RobotUtil::Transform2D *T);
        static void TransformationMult(RobotUtil::Pnt *pout, RobotUtil::Transform2D *Tin, Pnt *pin);
        static double CartesianDistance(double x1, double y1, double x2, double y2);
		static double getMinAngleError(double angle1, double angle2);

	private:
		static PowerDistributionPanel *util_power_panel;
};

/*******************************************************************************
 *
 ******************************************************************************/
class WashoutCommand
{
public:
	WashoutCommand();
	WashoutCommand(double coefficient, double decay_percent);
	~WashoutCommand();
	double setDecayPercent(double value);
	double setCoefficient(double value);
	double get() { return(value);};
	double update(double input);


private:
	double coeff;
	double decay_percent;
	double last_input;
	double last_decay;
	double value;

	bool initialized;
};


/*******************************************************************************
 *
 ******************************************************************************/
class TrapezoidalProfile
{
public:
	TrapezoidalProfile();
	TrapezoidalProfile(double max_vel, double perc_acc, double delta_t);
	~TrapezoidalProfile();
	double setMaxVelocity(double value);
	double setPercentAcc(double value);
	double setDeltaTime(double value);
	double getPos();
	double getVel();
	double getAcc();
	bool update();
	bool initialize(double current, double end);


private:
	double max_velocity;
	double percent_acc;
	double dt;
	double target_pos;
	double target_time;
	double time_1;
	double time_2;
	double running_time;
	bool initialized;

	double acc, last_acc;
	double vel, last_vel;
	double pos, last_pos;
	double max_acc;
};

/*******************************************************************************
 *
 ******************************************************************************/
class TrapezoidProfile2
{
public:
	TrapezoidProfile2();
	TrapezoidProfile2(double acc_percent, double decel_percent, double delta_t);
	~TrapezoidProfile2();
	void setPercentAcc(double acc, double decel);
	void setDeltaTime(double value);
	void setPos(double pos);
	double getPos();
	double getVel();
	double getAcc();
	double getEnd() { return(m_target_pos); };
	double getStart() { return(m_init_pos); };
	bool update();
	bool initialize(double end, double ave_vel, double start);
	void disable() { m_initialized = false; };
	void disable(double position);
	bool isRunning() { return m_initialized; };

private:
	double m_max_vel;
	double m_percent_acc;
	double m_percent_decel;

	double m_adjustment;
	double m_ave_vel;
	double m_dt;
	double m_target_pos;
	double m_init_pos;
	double m_target_time;
	double m_time_1;
	double m_time_2;
	double m_running_time;
	bool m_initialized;

	double m_acc, m_last_acc;
	double m_vel, m_last_vel;
	double m_pos, m_last_pos;
	double max_acc;

	double calcVel(double time_in, double max_vel, double time_1, double time_2, double end_time);
	void classInit(double acc_percent, double decel_percent, double delta_t);
};


/*******************************************************************************
 *
 * This class is used to move to create a trapezoidal trajectory profile that
 * will move a joint to a desired position in the minimum amount
 * of time without violating the specified max velocity, desired acceleration,
 * and desired deceleration limits
 *
 * Synopsis:
 *  // in the controls class definition
 *  TrapezoidProfile3 m_traj_profile;
 *
 *  // in the controls initialization
 *  m_traj_profile.setConfiguration(m_max_velocity, m_desired_acceleration, m_desired_deceleration, m_period);
 *
 *  // in the controls loop
 *  if (getPhase() == DISABLED)
 *   {
 *       target_position = actual_position;
 *       m_traj_profile.setInitialPosition(target_position);
 *   }
 *   else
 *   {
 *       if (button_pressed) target_position = 0; else target_position = 50;
 *
 *       m_traj_profile.setTargetPosition(target_position);
 *       motor->Set(m_traj_profile.update());  // motor is in position control mode
 *   }
 *
 ******************************************************************************/
class TrapezoidProfile3
{
    public:
        TrapezoidProfile3(void);
        ~TrapezoidProfile3(void);

        void setConfiguration(double max_velocity, double desired_acceleration, double desired_deceleration, double control_period);
        void setInitialPosition(double position);
        void setTargetPosition(double new_target);

        double update(void);
        double getTrajectoryPosition(void);
        double getTrajectoryVelocity(void);

    private:
        double m_target_position; // where we want to end

        double m_desired_position; // where we want to be in "period" time
        double m_desired_velocity;

        double m_max_velocity;
        double m_desired_decceleration;
        double m_desired_acceleration;
        double m_position_tolerance;
        double m_control_period; // the period at which we expect "update" to be called
};
