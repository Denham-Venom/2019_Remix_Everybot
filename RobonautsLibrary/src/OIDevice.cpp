/*******************************************************************************
 *
 * File: OIDevice.cpp
 * 
 * Written by:
 *     The Robonauts
 *     FRC Team 118
 *     NASA, Johnson Space Center
 *     Clear Creek Independent School District
 *
 ******************************************************************************/
#include <stdio.h>
#include <math.h>

#include "gsu/Advisory.h"

#include <limits>
#ifdef _2018
#include "Joystick.h"
#else
#include "frc/Joystick.h"
#endif

#include "RobonautsLibrary/OIDevice.h"
#include "RobonautsLibrary/RobotUtil.h"
#include "RobonautsLibrary/OIObserver.h"

using namespace frc;

/*******************************************************************************    
 * 
 * Create an instance of a an OI device
 * 
 ******************************************************************************/
OIDevice::OIDevice(std::string name, int a_channels, int d_channels, int i_channels, int s_channels) :
    device_name(name),
    ANALOG_CHANNELS(a_channels),
    DIGITAL_CHANNELS(d_channels),
    INT_CHANNELS(i_channels),
    STRING_CHANNELS(s_channels),
	INVALID_INT(std::numeric_limits<int>::lowest()),
	INVALID_FLOAT(std::numeric_limits<float>::lowest()),
	INVALID_STRING("KKLSKJFIFakdjfkdj"),
	INVALID_BOOL(255)
{    
    Advisory::pinfo("OIDevice::OIDevice() - creating %s with %d analogs, %d digitals, %d ints, and %d strings",
            name.c_str(), a_channels, d_channels, i_channels, s_channels);

    // 
    // This dynamic allocation of arrays for holding information
    // allows the arrays to be used by subclasses with different numbers
    // of channels.
    //
    analog_val  = new float[ANALOG_CHANNELS];
    digital_val = new uint8_t[DIGITAL_CHANNELS];
    int_val     = new int[INT_CHANNELS];
    string_val  = new std::string[STRING_CHANNELS];

    analog_observers  = new std::vector<OIDeviceObserver>[ANALOG_CHANNELS];
    digital_observers = new std::vector<OIDeviceObserver>[DIGITAL_CHANNELS];
    int_observers     = new std::vector<OIDeviceObserver>[INT_CHANNELS];
    string_observers  = new std::vector<OIDeviceObserver>[STRING_CHANNELS];

    // initialize all values
    //
    for (int i = 0; i < ANALOG_CHANNELS; i++)
    {
        analog_observers[i].clear();
        analog_val[i] = INVALID_FLOAT;
    }
    
    for (int i = 0; i < DIGITAL_CHANNELS; i++)
    {
        digital_observers[i].clear();
        digital_val[i] = INVALID_BOOL;
    }

    for (int i = 0; i < INT_CHANNELS; i++)
    {
        int_observers[i].clear();
        int_val[i] = INVALID_INT;
    }

    for (int i = 0; i < STRING_CHANNELS; i++)
    {
        string_observers[i].clear();
        string_val[i] = INVALID_STRING;
    }
}

/*******************************************************************************    
 *
 * Release any resources allocated by an instance of this class
 * 
 ******************************************************************************/
OIDevice::~OIDevice(void)
{
    if (analog_val != nullptr)
    {
        delete[] analog_val;
        analog_val = nullptr;
    }

    if (analog_observers != nullptr)
    {
        delete analog_observers;
        analog_observers = nullptr;
    }

    if (digital_val != NULL)
    {
        delete[] digital_val;
        digital_val = NULL;
    }

    if (digital_observers != nullptr)
    {
        delete digital_observers;
        digital_observers = nullptr;
    }

    if (int_val != NULL)
    {
        delete[] int_val;
        int_val = NULL;
    }

    if (int_observers != nullptr)
    {
        delete int_observers;
        int_observers = nullptr;
    }

    if (string_val != NULL)
    {
        delete[] string_val;
        string_val = NULL;
    }

    if (string_observers != nullptr)
    {
        delete string_observers;
        string_observers = nullptr;
    }
}

/*******************************************************************************    
 * 
 * Subscribe to receive an analog signal.
 * 
 * @param    chan        the device channel being subscribed to, note that
 *                         the channel is 0 based to match hardware labels.
 * 
 * @param    obs            a pointer to the observer that should be notified when
 *                         the specified channel changes
 *                 
 * @param    obs_data    an identifying value that gets passed back to the
 *                         observer with the new control value so the observer
 *                         can know what the new value represents.
 *                 
 * @param    obs_scale    the raw channel value will be multipled by this
 *                         scale before being passed to the observer, a value
 *                         of -1.0 can be used to invert the input
 *                 
 ******************************************************************************/
void OIDevice::subscribeAnalog(int chan, OIObserver *obs, int obs_data, 
    float obs_scale, float obs_deadband)
{
    if ((chan >= 0) && (chan < ANALOG_CHANNELS))
    {
		Advisory::pinfo("    subscribing to %s analog %d  scale = %5.2f",
			device_name.c_str(), chan, obs_scale);

		OIDeviceObserver new_obs;
		new_obs.obs = obs;
		new_obs.obs_data = obs_data;
		new_obs.adjust.scale = obs_scale;
		new_obs.deadband = obs_deadband;

		analog_observers[chan].push_back(new_obs);

		if(fabs(analog_val[chan] - INVALID_FLOAT) > 0.001)
		{
			obs->setAnalog(obs_data,
				RobotUtil::deadbandJoy(analog_val[chan], obs_deadband, 1.0) * obs_scale);
    	}
    }
}

/*******************************************************************************    
 * 
 * Subscribe to receive an int signal.
 *
 * @param    chan        the device channel being subscribed to, note that
 *                         the channel is 0 based to match hardware labels.
 *
 * @param    obs            a pointer to the observer that should be notified when
 *                         the specified channel changes
 *
 * @param    obs_data    an identifying value that gets passed back to the
 *                         observer with the new control value so the observer
 *                         can know what the new value represents.
 *
 * @param    obs_scale    the raw channel value will be multipled by this
 *                         scale before being passed to the observer, a value
 *                         of -1.0 can be used to invert the input.
 *
 ******************************************************************************/
void OIDevice::subscribeInt(int chan, OIObserver *obs, int obs_data,
    float obs_scale)
{
    if ((chan >= 0) && (chan < INT_CHANNELS))
    {
		Advisory::pinfo("    subscribing to %s int %d  scale = %5.2f",
			device_name.c_str(), chan, obs_scale);

		OIDeviceObserver new_obs;
		new_obs.obs = obs;
		new_obs.obs_data = obs_data;
		new_obs.adjust.scale = obs_scale;

		int_observers[chan].push_back(new_obs);

		if(int_val[chan] != INVALID_INT)
		{
			if (int_val[chan] >= 0)
			{
				obs->setInt(obs_data, (int)((int_val[chan] * obs_scale) + 0.49999999));
			}
			else
			{
				obs->setInt(obs_data, (int)((int_val[chan] * obs_scale) - 0.49999999));
			}
        }
    }
}

/*******************************************************************************
 *
 * Subscribe to receive an digital signal.
 * 
 * @param    chan        the device channel being subscribed to, note that
 *                         the channel is 0 based to match hardware labels.
 * 
 * @param    obs            a pointer to the observer that should be notified when
 *                         the specified channel changes
 *                 
 * @param    obs_data    an identifying value that gets passed back to the
 *                         observer with the new control value so the observer
 *                         can know what the new value represents.
 *             
 * @param    obs_invert    if the state of this input channel is opposite of
 *                      what is desired, an invert of true can be used
 *                      to flip it.
 *                  
 * N. B. This used to be 1-based values to handle joystick but then would
 *       be off by one for every other inherited class.  This has now been
 *       fixed and there's an overridden method in OIJoystick to handle its
 *       1-based-ness.
 *
 ******************************************************************************/
void OIDevice::subscribeDigital(int chan, OIObserver *obs, int obs_data,
    bool obs_invert)
{
    if ((chan >= 0) && (chan < DIGITAL_CHANNELS))
    {
        Advisory::pinfo("    subscribing to %s digital %d (0-Based!) invert = %s",
            device_name.c_str(), chan, (obs_invert?"true":"false"));

        OIDeviceObserver new_obs;
        new_obs.obs = obs;
        new_obs.obs_data = obs_data;
        new_obs.adjust.invert = obs_invert;

        digital_observers[chan].push_back(new_obs);
        if(digital_val[chan] != INVALID_BOOL)
        {
        	obs->setDigital(obs_data, digital_val[chan] != obs_invert);
        }
    }
    else
    {
    	Advisory::pcaution("   subscribe failed, asking for digital channel %d, only have %d", chan, DIGITAL_CHANNELS);
    }
}


/*******************************************************************************
 *
 * Subscribe to receive an string signal.
 * 
 * @param    chan        the device channel being subscribed to, note that
 *                       the channel is 0 based to match hardware labels.
 * 
 * @param    obs         a pointer to the observer that should be notified when
 *                       the specified channel changes
 *                 
 * @param    obs_data    an identifying value that gets passed back to the
 *                         observer with the new control value so the observer
 *                         can know what the new value represents.
 *             
 ******************************************************************************/
void OIDevice::subscribeString(int chan, OIObserver *obs, int obs_data)
{
    if ((chan >= 0) && (chan < STRING_CHANNELS))
    {
		Advisory::pinfo("    subscribing to %s string %d ",
			device_name.c_str(), chan);

		OIDeviceObserver new_obs;
		new_obs.obs = obs;
		new_obs.obs_data = obs_data;

		string_observers[chan].push_back(new_obs);

		if(string_val[chan] != INVALID_STRING)
		{
			obs->setString(obs_data, string_val[chan]);
    	}
    }
}

/*******************************************************************************    
 * 
 * Update all the specified channel with a new value.  If the new value is 
 * different from the previous value, the observers will be notified.
 * 
 * @param    idx        the channel index (zero based) of the value being updated
 * 
 * @param    val        the new value of the specified channel
 *                 
 ******************************************************************************/
void OIDevice::updateAnalog(int idx, float val)
{
	if(fabs(val - INVALID_FLOAT) > 0.001)
	{
		if (fabs(analog_val[idx] - val) > 0.001) // if the value changed by enough
		{
			if (val > 1.0) {val = 1.0;}
			if (val < -1.0) {val = -1.0;}

			for (OIDeviceObserver obs : analog_observers[idx])
			{
				obs.obs->setAnalog(obs.obs_data,
					RobotUtil::deadbandJoy(val, obs.deadband, 1.0) * obs.adjust.scale);
			}

			analog_val[idx] = val;
		}
	}
}

/*******************************************************************************    
 * 
 * Update all the specified channel with a new value.  If the new value is 
 * different from the previous value, the observers will be notified.
 * 
 * @param    idx        the channel index (zero based) of the value being updated
 * 
 * @param    val        the new value of the specified channel
 *                 
 ******************************************************************************/
void OIDevice::updateDigital(int idx, bool val)
{    
	if (digital_val[idx] != (uint8_t)val)
	{
		for(OIDeviceObserver obs : digital_observers[idx])
		{
			obs.obs->setDigital(obs.obs_data, val != obs.adjust.invert);
		}

		digital_val[idx] = (uint8_t)val;
	}
}

/*******************************************************************************
 *
 * Update all the specified channel with a new value.  If the new value is
 * different from the previous value, the observers will be notified.
 *
 * @param    idx        the channel index (zero based) of the value being updated
 *
 * @param    val        the new value of the specified channel
 *
 ******************************************************************************/
void OIDevice::updateInt(int idx, int val)
{
	if (int_val[idx] != val)
	{
		for (OIDeviceObserver obs : int_observers[idx])
		{
			if (val >= 0)
			{
				obs.obs->setInt(obs.obs_data,
						(int)((val * obs.adjust.scale) + 0.49999999)); // Truncation makes -1 go down to 0.
			}
			else
			{
				obs.obs->setInt(obs.obs_data,
						(int)((val * obs.adjust.scale) - 0.49999999)); // Truncation makes -1 go down to 0.
			}
		}

		int_val[idx] = val;

	}
}

/*******************************************************************************    
 * 
 * Update all the specified channel with a new value.  If the new value is 
 * different from the previous value, the observers will be notified.
 * 
 * @param    idx        the channel index (zero based) of the value being updated
 * 
 * @param    val        the new value of the specified channel
 *                 
 ******************************************************************************/
void OIDevice::updateString(int idx, std::string val)
{    
	if(val != INVALID_STRING)
	{
		if (strcmp(string_val[idx].c_str(), val.c_str()) != 0)
		{
			for(OIDeviceObserver obs : digital_observers[idx])
			{
				obs.obs->setString(obs.obs_data, val);
			}

			string_val[idx] = val;
		}
	}
}
