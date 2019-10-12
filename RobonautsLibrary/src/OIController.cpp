/*******************************************************************************
 *
 * File: OIController.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <iostream>

#include "gsu/Advisory.h"

#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/OIDevice.h"
#include "RobonautsLibrary/OIJoystick.h"
#include "RobonautsLibrary/OIGenericHID.h"
#include "RobonautsLibrary/OIDriverStation.h"

#if defined (FRC_CRIO)
#include "RobonautsLibrary/OICypress.h"
#endif

using namespace frc;

/*******************************************************************************	
 * 
 * Static allocations
 *  
 ******************************************************************************/
std::map<std::string, OIDevice *> OIController::device_map;

/*******************************************************************************	
 * 
 * Parse the XML and create any recognized devices
 * 
 * @param	xml	the xml that contains the OI Device definitions
 * 
 ******************************************************************************/
void OIController::configure(tinyxml2::XMLElement *xml)
{
	tinyxml2::XMLElement *comp;
	const char *type;
	const char *name;

	Advisory::pinfo("Creating Operator Interface Devices");

	std::map<std::string, OIDevice *>::iterator ittr;
	for (ittr = device_map.begin(); ittr != device_map.end(); ittr++)
	{
		delete (ittr->second);
	}
	device_map.clear();

	comp = xml->FirstChildElement("device");
	while (comp != NULL)
	{
		type = comp->Attribute("type");
		name = comp->Attribute("name");
		if ((type != NULL) && (name != NULL))
		{
#if defined (FRC_CRIO)
			if (strcmp(type, "cypress") == 0)
			{
				Advisory::pinfo("  creating cypress board %s", name);
				addDevice(name, new OICypress(name, DriverStation::GetInstance()));
			}
#endif
			if (strcmp(type, "joystick") == 0)
			{
				int num = comp-> IntAttribute("num");

				Advisory::pinfo("  creating joystick %s at %d", name, num);
				addDevice(name, new OIJoystick(name, num));
			}

			if (strcmp(type, "hid") == 0)
			{
				int analogs = comp->IntAttribute("analogs");
				int digitals = comp->IntAttribute("digitals");
				int povs = comp->IntAttribute("povs");
				int device_id = comp-> IntAttribute("num");

				Advisory::pinfo("  creating HID %s a:%d d:%d p:%d", name, analogs, digitals, povs);
				addDevice(name, new OIGenericHID(name, analogs, digitals, povs, device_id));
			}

			if (strcmp(type, "ds") == 0)
			{
				Advisory::pinfo("  creating DriverStation %s", name);

				addDevice(name, new OIDriverStation(name, DriverStation::GetInstance()));
			}

		}
		comp = comp->NextSiblingElement("device");
	}
}

/*******************************************************************************	
 * 
 ******************************************************************************/
void OIController::addDevice(std::string device, OIDevice *oi_device)
{
	device_map[device] = oi_device;
}

/*******************************************************************************	
 * 
 * Subscribe to receive an analog signal.
 * 
 * @param	xml			the XML element containing the device and channel
 * 
 * @param	obs			a pointer to the observer that should be notified when
 * 						the specified channel changes
 * 						
 * @param	obs_data	an identifying value that gets passed back to the
 * 						observer with the new control value so the observer
 * 						can know what the new value represents.
 * 						
 ******************************************************************************/
void OIController::subscribeAnalog(tinyxml2::XMLElement *xml,
    OIObserver *obs, int obs_data)
{
	float scale = 1.0;
	float deadband = 0.0;
	
	if (xml->Attribute("scale") != NULL)
	{
		scale = xml->FloatAttribute("scale");
	}
	else if (xml->BoolAttribute("invert"))
	{
		scale = -1.0;
	}

	deadband = xml->FloatAttribute("deadband");
	
    const char *device = xml->Attribute("device");
    if (device == nullptr)
    {
        Advisory::pcaution("Attempting connect to analog channel but device not specified");
    }

    int chan = -1;
    xml->QueryIntAttribute("chan", &chan);
    if (chan < 0)
    {
        Advisory::pcaution("Attempting connect to analog channel but chan was not found or invalid");
    }

	subscribeAnalog(device, chan, obs, obs_data, scale, deadband);
}

/*******************************************************************************	
 * 
 * Subscribe to receive an analog signal.
 * 
 * @param	name		the name of the device being subscribed to
 * 
 * @param	chan		the device channel being subscribed to
 * 
 * @param	obs			a pointer to the observer that should be notified when
 * 						the specified channel changes
 * 						
 * @param	obs_data	an identifying value that gets passed back to the
 * 						observer with the new control value so the observer
 * 						can know what the new value represents.
 * 	
 * @Pparam	obs_scale   the scale by which the analog value will be multipled
 *                      before being passed to the observer, -1.0 can be used
 *                      to invert the input
 *                      					
 ******************************************************************************/
void OIController::subscribeAnalog(std::string name, int chan,
    OIObserver *obs, int obs_data, float obs_scale, float obs_deadband)
{
	std::map<std::string, OIDevice *>::iterator ittr = device_map.find(name);
	if (ittr != device_map.end())
	{
		(ittr->second)->subscribeAnalog(chan, obs, obs_data, obs_scale, obs_deadband);
	}
}

/*******************************************************************************
 *
 * Subscribe to receive an int signal.
 *
 ******************************************************************************/
void OIController::subscribeInt(tinyxml2::XMLElement *xml,
		OIObserver *obs, int obs_data)
{
    float scale = 1.0f;
	xml->QueryFloatAttribute("scale", &scale);

    const char *device = xml->Attribute("device");
    if (device == nullptr)
    {
        Advisory::pcaution("Attempting connection to int channel but device not specified");
    }

    int chan = -1;
    xml->QueryIntAttribute("chan", &chan);
    if (chan < 0)
    {
        Advisory::pcaution("Attempting connection to int channel but chan was not found or invalid");
    }

    subscribeInt(device, chan, obs, obs_data, scale);
}

/*******************************************************************************
 *
 * Subscribe to receive an int signal.
 *
 ******************************************************************************/
void OIController::subscribeInt(std::string name, int chan,
				OIObserver *obs, int obs_data, float obs_scale)
{
	std::map<std::string, OIDevice *>::iterator ittr = device_map.find(name);

	if (ittr != device_map.end())
	{
		(ittr->second)->subscribeInt(chan, obs, obs_data, obs_scale);
	}
}

/*******************************************************************************	
 * 
 * Subscribe to receive an digital signal.
 * 
 * @param	xml			the XML element containing the device and channel
 * 
 * @param	obs			a pointer to the observer that should be notified when
 * 						the specified channel changes
 * 						
 * @param	obs_data	an identifying value that gets passed back to the
 * 						observer with the new control value so the observer
 * 						can know what the new value represents.
 * 						
 ******************************************************************************/
void OIController::subscribeDigital(tinyxml2::XMLElement *xml,
    OIObserver *obs, int obs_data)
{
    int chan = -1;
    bool invert = false;

    const char *device = xml->Attribute("device");
    xml->QueryIntAttribute("chan", &chan);
    xml->QueryBoolAttribute("invert", &invert);

    if (device == nullptr)
    {
        Advisory::pcaution("Attempting connection to digital channel but device not specified");
    }

    if (chan < 0)
    {
        Advisory::pcaution("Attempting connection to digital channel but chan was not found or invalid");
    }

    subscribeDigital(device, chan, obs, obs_data, invert);
}

/*******************************************************************************	
 * 
 * Subscribe to receive a digital signal.
 * 
 * @param	name		the name of the device being subscribed to
 * 
 * @param	chan		the device channel being subscribed to
 * 
 * @param	obs			a pointer to the observer that should be notified when
 * 						the specified channel changes
 * 						
 * @param	obs_data	an identifying value that gets passed back to the
 * 						observer with the new control value so the observer
 * 						can know what the new value represents.
 * 						
 * @param	obs_invert	invert the state of the digital input
 * 
 ******************************************************************************/
void OIController::subscribeDigital(std::string name, int chan,
    OIObserver *obs, int obs_data, bool obs_invert)
{
	std::map<std::string, OIDevice *>::iterator ittr = device_map.find(name);
	if (ittr != device_map.end())
	{
		(ittr->second)->subscribeDigital(chan, obs, obs_data, obs_invert);
	}
	else
	{
        Advisory::pcaution("Attempting connection to digital channel but device %s not known", name.c_str());
	}
}

/*******************************************************************************	
 *
 * Subscribe to receive an string signal.
 *
 * @param	xml			the XML element containing the device and channel
 *
 * @param	obs			a pointer to the observer that should be notified when
 * 						the specified channel changes
 *
 * @param	obs_data	an identifying value that gets passed back to the
 * 						observer with the new control value so the observer
 * 						can know what the new value represents.
 *
 ******************************************************************************/
void OIController::subscribeString(tinyxml2::XMLElement *xml,
    OIObserver *obs, int obs_data)
{
    int chan = -1;

    const char *device = xml->Attribute("device");
    xml->QueryIntAttribute("chan", &chan);

    if (device == nullptr)
    {
        Advisory::pcaution("Attempting connection to digital channel but device not specified");
    }

    if (chan < 0)
    {
        Advisory::pcaution("Attempting connection to digital channel but chan was not found or invalid");
    }

    subscribeString(device, chan, obs, obs_data);
}

/*******************************************************************************
 *
 * Subscribe to receive a string signal.
 *
 * @param	name		the name of the device being subscribed to
 *
 * @param	chan		the device channel being subscribed to
 *
 * @param	obs			a pointer to the observer that should be notified when
 * 						the specified channel changes
 *
 * @param	obs_data	an identifying value that gets passed back to the
 * 						observer with the new control value so the observer
 * 						can know what the new value represents.
 *
 ******************************************************************************/
void OIController::subscribeString(std::string name, int chan,
    OIObserver *obs, int obs_data)
{
	std::map<std::string, OIDevice *>::iterator ittr = device_map.find(name);
	if (ittr != device_map.end())
	{
		(ittr->second)->subscribeString(chan, obs, obs_data);
	}
}

/*******************************************************************************
 *
 * This update method should be called for each loop through the main code or
 * each time a new packet is received from the drivers station.  It loops
 * through all IO devices to make sure they notify any observers about
 * changes. 
 *  
 ******************************************************************************/
void OIController::update(void)
{
	std::map<std::string, OIDevice *>::iterator ittr;
	for (ittr = device_map.begin(); ittr != device_map.end(); ittr++)
	{
		(ittr->second)->update();
	}
}
