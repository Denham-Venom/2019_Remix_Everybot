/*******************************************************************************
 *
 * File: PositionJointSRX.h
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
#include "RobonautsLibrary/MacroStep.h"

/*******************************************************************************	
 * 
 * Create an instance of a position joint srx and connect it to the specified
 * motor and inputs
 * 
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 * 
 *  <control type="position joint srx" [name="unnamed"] [closed_loop="false"] [period="0.1"]
 *  		[setpoint0="0.0"] [setpoint1="0.0"] [setpoint2="0.0"] [setpoint3="0.0"] [setpoint4="0.0"]
 *  		[setpoint5="0.0"] [setpoint6="0.0"]  >
 *
 *  	<motor name="front_right" [type="CanTalon"] [port="2"] [control_period=”10”] [invert="false"]>
 *   		[<encoder [invert=”false”] [scale=”1.0”] />]
 *  		[<pid [kf=”0.001”] [kp=”0.0”] [ki=”0.0”] [kd=”0.0”] />]
 *		</motor>
 *
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
 * 		[<oi name="setpoint6" device="pilot" chan="3" [invert="false"]/>]
 *   
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
class PositionJointSRX : public PeriodicControl, public OIObserver
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
			CMD_SETPOINT_IDX,
			CMD_INCREMENT_POS,
			CMD_DECREMENT_POS,
		};


		static const uint8_t NUM_SETPOINTS = 7;
		static const uint8_t NUM_MOMENTARIES = 4;

		PositionJointSRX(std::string control_name, tinyxml2::XMLElement *xml);
		~PositionJointSRX(void);

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

		void setClosedLoop(bool closed);
		bool isClosedLoop(void);
		int8_t getSetpointIndex(std::string setpoint_name);

	private:		
		void applyMomentary(bool on, int idx);
		void applySetpoint(bool on, int idx);

		RSpeedController	*motor;
		DataLogger      *pjs_log;

		TrapezoidProfile3 m_traj_profile;

		float m_max_velocity;
		float m_desired_acceleration;
		float m_desired_deceleration;

		float increment_step;
		float decrement_step;
		float delta_position;
		
		bool m_is_ready;
		float motor_scale;

		float raw_position;
		float actual_position;
		float actual_velocity;
		int32_t raw_velocity;

		float target_power;
		float target_position;
		float command_power;
		float max_power_delta;
		float commanded_position;

		std::string m_setpoint_name[NUM_SETPOINTS];

		uint8_t setpoint_index;
		uint8_t num_setpoints;
		float m_pjs_setpoint[NUM_SETPOINTS];

		float momentary_power[NUM_MOMENTARIES];
		float momentary_step[NUM_MOMENTARIES];

		bool m_closed_loop;

};
class MSPosJointSRXPBCSetPower : public MacroStepSequence
{
	public:
	MSPosJointSRXPBCSetPower(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		PositionJointSRX *m_parent_control;

		float m_power;
};
