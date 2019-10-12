/*******************************************************************************
 *
 * File: OIController.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <iostream>
#include <map>

#include "gsu/tinyxml2.h"

#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIDevice.h"
#include "RobonautsLibrary/OIJoystick.h"

/*******************************************************************************
 *
 * The OI Controller holds a reference to each of the OI Devices used by this
 * robot so the components of the robot can register to receive events from 
 * a single interface.
 * 
 ******************************************************************************/
class OIController
{
	public:
		static void configure(tinyxml2::XMLElement *xml);

		static void addDevice(std::string device, OIDevice *oi_device);
		
		static void subscribeAnalog(tinyxml2::XMLElement *xml, 
			OIObserver *obs, int obs_data);
		
		static void subscribeAnalog(std::string device, int chan, 
			OIObserver *obs, int obs_data, float obs_scale, float obs_deadband);
		
		static void subscribeInt(tinyxml2::XMLElement *xml,
				OIObserver *obs, int obs_data);

		static void subscribeInt(std::string device, int chan,
				OIObserver *obs, int obs_data, float obs_scale = 1.0f);

		static void subscribeDigital(tinyxml2::XMLElement *xml, 
			OIObserver *obs, int obs_data);
		
		static void subscribeDigital(std::string device, int chan, 
			OIObserver *obs, int obs_data, bool obs_invert);

		static void subscribeString(tinyxml2::XMLElement *xml, OIObserver *obs, int obs_data);
		static void subscribeString(std::string device, int chan, OIObserver *obs, int obs_data);
		
		// For completeness, there should be an unsubscribe but
		// we don't use it so there's not
		// static void unsubscribeAll(OIObserver *obs);
		
		static void update(void);

	private:		
		static std::map<std::string, OIDevice *> device_map;
};

