/*******************************************************************************
 *
 * File: ServoControl.h
 *
 * Control of a single Servo or a double Servo
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
#include "Servo.h"
#else
#include "frc/Servo.h"
#endif


#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"


/*******************************************************************************
 *
 * Instances of this class can be created to control either a single Servo
 * or a double Servo.  If specified, the second Servo control will be
 * set to the opposite state of the first Servo.
 * 
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="servo" [name="unnamed"] [period="0.1"] step_size="5" up_position="10" down_position="-10">
 * 		<servo [name="a"] module="1" port="1" />
 *      [<oi name="servo_up"     device="pilot" chan="1" [invert="false"]/>]
 *      [<oi name="servo_down"    device="pilot" chan="2" [invert="false"]/>]
 *      [<oi name="servo_toggle" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="servo_state"  device="oi"    chan="1" [invert="false"]/>]
 *  </control>
 * 
 ******************************************************************************/
class ServoControl : public PeriodicControl, public OIObserver
{
	public:
		enum {CMD_UP, CMD_DOWN, CMD_TOGGLE, CMD_STATE, CMD_VELOCITY};

		ServoControl(std::string control_name, tinyxml2::XMLElement *xml);
		~ServoControl(void);

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

	private:
		Servo	*m_servo;

		float m_servo_target_position;
		float m_servo_step_size;
		float m_servo_up_position;
		float m_servo_down_position;
		float m_servo_target_velocity;
};

