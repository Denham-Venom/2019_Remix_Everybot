/*******************************************************************************
 *
 * File: SolenoidControl.h
 *
 * Control of a single solenoid or a double solenoid
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
#include "Solenoid.h"
#else
#include "frc/Solenoid.h"
#endif


#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"

/*******************************************************************************
 *
 * Instances of this class can be created to control either a single solenoid
 * or a double solenoid.  If specified, the second solenoid control will be
 * set to the opposite state of the first solenoid.
 * 
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="solenoid" [name="unnamed"] [period="0.1"] >
 * 		<solenoid [name="a"] module="1" port="1" />
 * 		[<solenoid [name="b"] module="1" port="2" />]
 *      [<oi name="on"     device="pilot" chan="1" [invert="false"]/>]
 *      [<oi name="off"    device="pilot" chan="2" [invert="false"]/>]
 *      [<oi name="toggle" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="state"  device="oi"    chan="1" [invert="false"]/>]
 *  </control>
 * 
 ******************************************************************************/
class SolenoidControl : public PeriodicControl, public OIObserver
{
	public:
		enum {CMD_ON, CMD_OFF, CMD_TOGGLE, CMD_STATE};

		SolenoidControl(std::string control_name, tinyxml2::XMLElement *xml);
		~SolenoidControl(void);

  		void controlInit(void);
		void updateConfig(void);

		void disabledInit();
		void autonomousInit();
		void teleopInit();
		void testInit();
		void doPeriodic();

		void publish(void);
		void setState(bool state);

		void setAnalog(int id, float val);
		void setDigital(int id, bool val);
		void setInt(int id, int val);

	private:
		Solenoid *solenoid_a;
		Solenoid *solenoid_b;

		bool solenoid_state;
};

/*******************************************************************************
 *
 * This Macro Step sets the Solenoid State.
 *
 *
 ******************************************************************************/
class MSSolenoidPBCSetState : public MacroStepSequence
{
	public:
		MSSolenoidPBCSetState(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		SolenoidControl *m_parent_control;

		bool m_state;
};
