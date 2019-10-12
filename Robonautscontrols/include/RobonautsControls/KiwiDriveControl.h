/*******************************************************************************
 *
 * File: KiwiDriveControl.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"

/*******************************************************************************
 *
 *  Provides a PeriodicControl implementation for driving a three omni-wheel
 *  robot base.
 *  
 *  There are wheels on the front left, front right, and back of the robot.
 *  +X is forward, +Y is left, +Z is up, looking down on the robot, positive
 *  rotation is counter clockwise (or to the left) -- use the right-hand rule.
 *
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="kiwi_drive" name="drive">
 *      [<motor name="front_left"   [type="Victor"] [port="1"] [invert="false"] />]
 *      [<motor name="front_right"  [type="Victor"] [port="2"] [invert="false"] />]
 *      [<motor name="back"  	    [type="Victor"] [port="3"] [invert="false"] />]
 *
 *      [<oi name="forward"    	device="pilot" chan="1" 	[scale="-0.8"]/>]
 *      [<oi name="lateral"     device="pilot" chan="0"     [scale="0.8"]/>]
 *      [<oi name="rotate"      device="pilot" chan="3"     [scale="0.8"]/>]
 *  </control>
 * 
 ******************************************************************************/
class KiwiDriveControl : public PeriodicControl, public OIObserver
{
	public:
		enum
		{
			CMD_FORWARD = 0, CMD_LATERAL, CMD_ROTATE
		};

		enum
		{
		    MOTOR_FRONT_LEFT = 0, MOTOR_FRONT_RIGHT, MOTOR_BACK, NUM_MOTORS
		};

		enum
		{
		    INPUT_FORWARD = 0, INPUT_LATERAL, INPUT_ROTATE, NUM_INPUTS
		};

		KiwiDriveControl(std::string name, tinyxml2::XMLElement *xml);
		~KiwiDriveControl(void);

		void setAnalog(int id, float val);
		void setDigital(int id, bool val);
		void setInt(int id, int val);

		void controlInit();

		void disabledInit();
		void autonomousInit();
		void teleopInit();
		void testInit();

		void doPeriodic();
		void publish();

	private:
		RSpeedController *m_drive_motor[NUM_MOTORS];

		float m_motor_cmd[NUM_MOTORS];
		float m_motor_trg[NUM_MOTORS];

		float m_input_cmd[NUM_INPUTS];
};


// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 * This Macro Step sets the forward power, lateral power, and rotate power of the
 * Kiwi Arcade PacBotControl.
 *
 * WARNING: This just sets the power, it does not turn them off unless the
 *          provided values for forward, lateral, and rotate turn the drive off.
 *
 *  Example XML:
 *
 *	<step name="drive_1" control="drive" type="DrivePower"
 *			forward="0.5" lateral="0.1" rotate="false">
 *  	<connect type="next" step="drive_wait"/>>
 *  </step>
 *
 ******************************************************************************/
class MSKiwiDrivePower : public MacroStepSequence
{
	public:
		MSKiwiDrivePower(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		KiwiDriveControl *parent_control;

		float m_forward;
		float m_lateral;
		float m_rotate;
};
