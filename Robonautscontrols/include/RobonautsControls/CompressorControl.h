/*******************************************************************************
 *
 * File: CompressorControl.h
 *
 * This class is to control a compressor using a relay and pressure switch
 * without using the logic built into the PCM. Concerns about drawing too
 * much current through the PCM and fuse shared with the radio lead to
 * the creation of this class.
 *
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 *
 *
 ******************************************************************************/
#pragma once

#include "gsu/tinyxml2.h"

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/XmlRobotUtil.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"

/*******************************************************************************
 *
 * This control is for a compressor that is connected with a pressure switch
 * and relay.
 *
 * XML Example:
 *   <control type="compressor" name="Compressor" [period="0.1"] [priority="0"]>
 *  	<digital_input   [module="1"] [port="1"] [...] />
 *  	<relay           [module="1"] [port="1"] [fuse="-1"] [...] />
 *   </compressor>
 *
 ******************************************************************************/
class CompressorControl: public PeriodicControl
{
	public:
		CompressorControl(std::string name, tinyxml2::XMLElement *xml = NULL);
		~CompressorControl();

		void publish(void);

	protected:
		void controlInit();
		void updateConfig();

		void disabledInit();
		void autonomousInit();
		void teleopInit();
		void testInit();

		void doPeriodic();

	private:
		Relay *compressor_relay;
		DigitalInput *compressor_switch;

		bool compressor_switch_invert;
		bool compressor_on;

		int compressor_fuse;
		float compressor_current;
};
