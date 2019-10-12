/*******************************************************************************
 *
 * File: OIDevice.h
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

#include "RobonautsLibrary/OIObserver.h"

/*******************************************************************************
 *
 ******************************************************************************/
class   OIDeviceObserver
{
	public:
		OIObserver *obs;
		int obs_data;

		union
		{
			float scale;
			bool invert;
		} adjust;

		float deadband;
};

/*******************************************************************************
 *
 * This base class is the base for all IO Devices, such as OICypress
 * and OIJoystick
 * 
 * At this time there is a limitation in that only one observer can subscribe
 * to each channel.  In the future I would like to change this by having a
 * linked list or std::vector of observer records for each channel
 *
 ******************************************************************************/
class OIDevice
{
	public:
		OIDevice(std::string name, int analog_channels, int digital_channels, int int_channels = 0, int string_channels = 0);
		virtual ~OIDevice();
		
		virtual void subscribeAnalog(int chan, OIObserver *obs, int obs_data, 
			float obs_scale = 1.0, float obs_deadband = 0.0);
		
		virtual void subscribeInt(int chan, OIObserver *obs, int obs_data,
			float obs_scale = 1.0f);

		virtual void subscribeDigital(int chan, OIObserver *obs, int obs_data,
			bool invert = false);

		virtual void subscribeString(int chan, OIObserver *obs, int obs_data);
		
		// For completeness there should an unsubscribe, but
		// we won't use it so it's not there
		//virtual void unsubscribeAll(OIObserver *obs);
		
		virtual void update() = 0;
		
	protected:
		virtual void updateAnalog(int idx, float val);
		virtual void updateDigital(int idx, bool val);
		virtual void updateInt(int idx, int val);
		virtual void updateString(int idx, std::string val);
		
		const std::string device_name;
		// the number of each type of channel
		const int ANALOG_CHANNELS;
		const int DIGITAL_CHANNELS;
		const int INT_CHANNELS;
		const int STRING_CHANNELS;

		const int INVALID_INT;
		const float INVALID_FLOAT;
		const std::string INVALID_STRING;
		const uint8_t INVALID_BOOL;

	    // arrays of values, one value for each channel
		float  *analog_val;
		uint8_t   *digital_val;
		int    *int_val;
		std::string *string_val;

		// arrays of vectors, one vector for each channel
		std::vector<OIDeviceObserver> *analog_observers;
		std::vector<OIDeviceObserver> *digital_observers;
		std::vector<OIDeviceObserver> *int_observers;
		std::vector<OIDeviceObserver> *string_observers;

};
