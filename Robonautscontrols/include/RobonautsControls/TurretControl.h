/*******************************************************************************
 *
 * File: TurretControl.h
 * 
 * Motor with Closed Loop control, feedback from a Pot and limit switches
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

//#include "frc/SpeedController.h"
#include "RobonautsLibrary/RSpeedController.h"
#include "RobonautsLibrary/RPot.h"
#include "RobonautsLibrary/RAbsPosSensor.h"
#include "RobonautsLibrary/SimplePID.h"
#include "RobonautsLibrary/RobotUtil.h"
#include "RobonautsLibrary/DataLogger.h"

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"

/*******************************************************************************	
 * 
 * Create an instance of a turret control and connect it to the specified
 * motor and inputs
 * 
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 * 
 *  <control type="turret" [name="unnamed"] [closed_loop="false"] [period="0.1"]
 *  		[setpoint0="0.0"] [setpoint1="0.0"] [setpoint2="0.0"] [setpoint3="0.0"] [setpoint4="0.0"]
 *  		[setpoint5="0.0"] [setpoint6="0.0"] [setpoint7="0.0"] [setpoint8="0.0"] [setpoint9="0.0"]  >
 *
 *      [<motor [type="Victor"] [module="1"] [port="1"] [invert="false"] />]
 *
 *      [
 *        <aps port="port" p1_raw, p1_cal, p2_raw, p2_cal />
 *
 *        <pot [module="module"] [port="port"] [p1_raw, p1_cal, p2_raw, p2_cal]
 *    		[min_raw="min_raw"] [max_raw="max_raw"] />
 *
 *        <pid [kp="kp"] [ki="ki"] [kd="kd"]
 *  	 	[targ_min="targ_min"] [targ_max="targ_max"] [targ_thp="targ_thp"]
 *  	 	[cntl_min="cntl_min"] [cntl_max="cntl_max"] />
 *  	]
 * 
 *    	[<digital_input name="upper_limit" port="0" [invert="false"]/>]
 *    	[<digital_input name="lower_limit" port="1" [invert="false"]/>]
 *
 *      [<oi name="closed_loop_state"  device="switches" chan="1" [invert="false"]/>]
 *
 *      [<oi name="analog"    device="pilot" chan="1" [scale="1.0"|invert="false"]/>]
 *      [<oi name="increment" step="0.1" device="pilot" chan="5" [invert="false"]/>]
 *      [<oi name="decrement" step="0.1" device="pilot" chan="7" [invert="false"]/>]
 *      [<oi name="stop"      device="pilot" chan="1" [invert="false"]/>]
 *
 *      [<oi name="setpoint_idx" device="pilot" chan="0" [scale="0.2222222"]/>
 *
 *      [<oi name="setpoint0" device="pilot" chan="2" [invert="false"]/>]
 *      [<oi name="setpoint1" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint2" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint3" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint4" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint5" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint6" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint7" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint8" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="setpoint9" device="pilot" chan="3" [invert="false"]/>]
 *
 *      [<oi name="momentary0" device="pilot" chan="5" ol_power="0.6" cl_step="0.1" [invert="false"]/>]
 *      [<oi name="momentary1" device="pilot" chan="7" ol_power="-0.6" cl_step="-0.1" [invert="false"]/>]
 *      [<oi name="momentary2" device="pilot" chan="6" ol_power="1.0" cl_step="0.2" [invert="false"]/>]
 *      [<oi name="momentary3" device="pilot" chan="8" ol_power="-1.0" cl_step="-0.2" [invert="false"]/>]
 *
 *  </control>
 *  
 *  Note: either pot or aps can be used, if both are specified the pot will be
 *        used.
 *
 ******************************************************************************/
class TurretControl : public PeriodicControl, public OIObserver
{
	public:
		enum Command
		{
			CMD_CLOSED_LOOP_STATE = 0,
			CMD_MOVE_ANALOG,
			CMD_MOMENTARY_0,
			CMD_MOMENTARY_1,
			CMD_MOMENTARY_2,
			CMD_MOMENTARY_3,
			CMD_SETPOINT_0,
			CMD_SETPOINT_1,
			CMD_SETPOINT_2,
			CMD_SETPOINT_3,
			CMD_SETPOINT_4,
			CMD_SETPOINT_5,
			CMD_SETPOINT_6,
			CMD_SETPOINT_7,
			CMD_SETPOINT_8,
			CMD_SETPOINT_9,
			CMD_SETPOINT_IDX,
			CMD_INCREMENT_POS,
			CMD_DECREMENT_POS
		};

		static const uint8_t NUM_SETPOINTS = 10;
		static const uint8_t NUM_MOMENTARIES = 4;

		TurretControl(std::string control_name, tinyxml2::XMLElement *xml);
		~TurretControl(void);

  		void controlInit(void);
		void updateConfig(void);

		void disabledInit();
		void autonomousInit();
		void teleopInit();
		void testInit();
		void doPeriodic();

		void publish(void);

		void setAnalog(int id, float val);
		void setDigital(int id, bool val);
		void setInt(int id, int val);

		void setPosition(float val);
		float getPostion(void);

		void initLogFile(void);

		bool isClosedLoop(void);
		
	private:		
		void applyMomentary(bool on, int idx);
		void applySetpoint(bool on, int idx);

		RSpeedController	*motor;
		RPot 			*pot;
		RAbsPosSensor   *aps;
		SimplePID 		*pid;
		DigitalInput 	*upper_limit_sw;
		DigitalInput 	*lower_limit_sw;
		DataLogger      *turret_log;

		TrapezoidProfile3 m_traj_profile;

		float m_max_velocity;
		float m_desired_acceleration;
		float m_desired_deceleration;

		float increment_step;
		float decrement_step;
		float delta_position;
		

		float motor_scale;
		bool upper_limit_invert;
		bool lower_limit_invert;

		bool upper_limit_pressed;
		bool lower_limit_pressed;

		bool pot_ready;
		float raw_position;
		float actual_position;
		float actual_velocity;
		int32_t raw_velocity;

		float target_power;
		float target_position;
		float command_power;
		float max_power_delta;

		float setpoint[NUM_SETPOINTS];
		uint8_t setpoint_index;
		uint8_t num_setpoints;

		float momentary_power[NUM_MOMENTARIES];
		float momentary_step[NUM_MOMENTARIES];

		bool closed_loop;
};


//class MSMotorCLPosSetPosition : public MacroStepSequence
//{
//	public:
//		MSMotorCLPosSetPosition(std::string type, tinyxml2::XMLElement *xml, void *control);
//
//		void init(void);
//		MacroStep * update(void);
//
//	private:
//		MotorCLPos *motor_control;
//		float position;
//		float tolerance;
//};
