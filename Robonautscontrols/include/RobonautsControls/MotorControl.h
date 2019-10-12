/*******************************************************************************
 *
 * File: MotorControl.h
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
#include "SpeedController.h"
#else
#include "frc/SpeedController.h"
#endif

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"
#include "RobonautsLibrary/RSpeedController.h"
#include "RobonautsLibrary/RDigitalInput.h"
/*******************************************************************************	
 * 
 * Create an instance of an open loop motor control and connect it to the specified
 * motor and inputs
 * 
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="motor" [name="unnamed"] [max_cmd_delta="0.25"] [period="0.1"]
 *      [min_control="-1.0"] [max_control="1.0"] >
 *
 *      [<motor [type="Victor"] [module="1"] [port="1"] [invert="false"] />]
 * 		[<motor [type="Victor"] [module="1"] [port="1"] [invert="false"] />]
 * 		[<digital_input name="upper_limit" port="1" [normally_open="false"] />]
 * 		[<digital_input name="lower_limit" port="2" [normally_open="false"] />]
 *      [<oi name="analog"    	device="pilot" chan="1" [scale="1.0"|invert="false"]/>]
 *      [<oi name="increment" 	device="pilot" chan="2" step="0.1" [invert="false"]/>]
 *      [<oi name="decrement" 	device="pilot" chan="3" step="0.1" [invert="false"]/>]
 *      [<oi name="stop"      	device="pilot" chan="4" [invert="false"]/>]
 *      [<oi name="momentary_a"	device="pilot" chan="4" value="0.1" [invert="false"]/>]
 *      [<oi name="momentary_b"	device="pilot" chan="4" value="0.1" [invert="false"]/>]
 *      [<oi name="momentary_c"	device="pilot" chan="4" value="0.1" [invert="false"]/>]
 *      [<oi name="momentary_d"	device="pilot" chan="4" value="0.1" [invert="false"]/>]
 *  </control>
 *
 ******************************************************************************/
class MotorControl : public PeriodicControl, public OIObserver
{
	public:
		enum {CMD_ANALOG=0, CMD_INCREMENT, CMD_DECREMENT, CMD_STOP, CMD_MOMENTARY_A
			,CMD_MOMENTARY_B ,CMD_MOMENTARY_C, CMD_MOMENTARY_D};
		
		MotorControl(std::string control_name, tinyxml2::XMLElement *xml);
		~MotorControl(void);

  		void controlInit(void);
		void updateConfig(void);

		void disabledInit();
		void autonomousInit();
		void teleopInit();
		void testInit();
		void doPeriodic();

		void setAnalog(int id, float val);
		void setDigital(int id, bool val);
		void setInt(int id, int val);

		void publish(void);

	private:		
		RSpeedController *motor_a;
		RSpeedController *motor_b;

		RDigitalInput *m_upper_limit_sw;
		RDigitalInput *m_lower_limit_sw;

		bool m_upper_limit_pressed;
		bool m_lower_limit_pressed;

		float motor_min_control;
		float motor_max_control;

        uint32_t motor_a_max_current;
        uint32_t motor_b_max_current;

		float motor_increment_step;
		float motor_decrement_step;

		float motor_momentary_a_value;
		float motor_momentary_b_value;
		float motor_momentary_c_value;
		float motor_momentary_d_value;

		float motor_max_cmd_delta;

		float motor_target_power;
		float motor_command_power;
};

/*******************************************************************************
 *
 * This Macro Step sets the power Motor PacBotControl.
 *
 * WARNING: This just sets the power, it does not turn it off unless the
 *          provided value for power is 0.0.
 *
 *  Example XML:
 *
 *	<step name="lift_up" control="motor_1" type="SetPower"	power="0.5" >
 *  	<connect type="next" step="lift_up_wait"/>>
 *  </step>
 *
 ******************************************************************************/
class MSMotorPBCSetPower : public MacroStepSequence
{
	public:
	MSMotorPBCSetPower(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		MotorControl *m_parent_control;

		float m_power;
};
