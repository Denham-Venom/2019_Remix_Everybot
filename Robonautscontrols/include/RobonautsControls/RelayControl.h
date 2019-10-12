/*******************************************************************************
 *
 * File: RelayControl.h
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
#include "Relay.h"
#include "smartdashboard/SmartDashboard.h" //WPI
#else
#include "frc/Relay.h"
#include "frc/smartdashboard/SmartDashboard.h" //WPI
#endif

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"

/*******************************************************************************	
 * 
 * Create an instance of a relay and connect it to the specified
 * relay and inputs
 * 
 * This class is desigend to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="relay" [name="unnamed"] [period="0.1"] >
 *      <relay [module="1"] [port="1"] />
 *      [<oi name="on"    device="pilot" chan="1" [invert="false"]/>]
 *      [<oi name="off" device="pilot" chan="2" [invert="false"]/>]
 *      [<oi name="forward" device="pilot" chan="3" [invert="false"]/>]
 *      [<oi name="reverse" device="pilot" chan="4" [invert="false"]/>]
 *  </control>
 * 
 ******************************************************************************/
class RelayControl : public PeriodicControl, public OIObserver
{
	public:
		enum {CMD_OFF = 0,CMD_ON, CMD_FORWARD, CMD_REVERSE };

		RelayControl(std::string control_name, tinyxml2::XMLElement *xml);
		~RelayControl(void);

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
		frc::Relay * relay;
		frc::Relay::Value relay_value;
};

/*******************************************************************************
 *
 * This Macro Step sets the
 *
 * WARNING:
 *
 *  Example XML:
 *
 *	<step name="" control="" type=""	="0.5" >
 *  	<connect type="next" step="lift_up_wait"/>>
 *  </step>
 *
 ******************************************************************************/
class MSRelaySetState : public MacroStepSequence
{
	public:
	MSRelaySetState(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		RelayControl *m_parent_control;
        int m_cmd_id;
		bool m_relay_value;
};
