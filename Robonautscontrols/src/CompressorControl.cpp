/*******************************************************************************
 *
 * File: CompressorControl.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <math.h>

#include "RobonautsLibrary/RobotUtil.h"

#include "gsu/Advisory.h"

#include "RobonautsControls/CompressorControl.h"

#ifdef _2018
#include "smartDashboard/SmartDashboard.h"
#else
#include "frc/smartDashboard/SmartDashboard.h"
#endif

using namespace tinyxml2;
using namespace gsi;

/******************************************************************************
 *
 *
 *
 ******************************************************************************/
CompressorControl::CompressorControl(std::string name, tinyxml2::XMLElement *xml)
	: PeriodicControl(name)
{
	XMLElement *element;

	Advisory::pinfo("========================= Creating Compressor Control [%s] =========================",
			name.c_str());

	compressor_relay = nullptr;
	compressor_switch = nullptr;

	compressor_switch_invert = false;
	compressor_on = false;
	compressor_fuse = -1;
	compressor_current = 0.0;

	//
	// Parse the XML
	//

	element = xml->FirstChildElement("digital_input");
	if (element != nullptr)
	{
		Advisory::pinfo("  creating switch for compressor");
		compressor_switch = XmlRobotUtil::createDigitalInput(element);
		compressor_switch_invert = element->BoolAttribute("invert") ? true : false;
	}

	element = xml->FirstChildElement("relay");
	if (element != NULL)
	{
		Advisory::pinfo("  creating compressor relay");
		element->QueryIntAttribute("fuse", &compressor_fuse);
		compressor_relay = XmlRobotUtil::createRelay(element);
	}

	if (compressor_switch == nullptr)
	{
		Advisory::pinfo("  WARNING: failed to initialize compressor switch");
	}

	if (compressor_relay == nullptr)
	{
	    Advisory::pinfo("  WARNING: failed to create compressor relay");
	}
}

/******************************************************************************
 *
 * Destructor
 * 
 ******************************************************************************/
CompressorControl::~CompressorControl()
{
    Advisory::pinfo("CompressorSys::~CompressorSys");
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 * 
 ******************************************************************************/
void CompressorControl::controlInit()
{

}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void CompressorControl::updateConfig()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void CompressorControl::disabledInit()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void CompressorControl::autonomousInit()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void CompressorControl::teleopInit()
{
}

/**********************************************************************
 *
 *
 **********************************************************************/
void CompressorControl::testInit(void)
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 * 
 ******************************************************************************/
void CompressorControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	SmartDashboard::PutBoolean(getName() + " on: ", (compressor_switch != nullptr) && compressor_on);
	SmartDashboard::PutNumber(getName() + " current: ", compressor_current);
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 * 
 ******************************************************************************/
void CompressorControl::doPeriodic()
{
	if (compressor_switch != nullptr)
	{
		compressor_on = compressor_switch->Get() != compressor_switch_invert;
	}

	if (compressor_fuse != 0xFF)
	{
		compressor_current = RobotUtil::getCurrent(compressor_fuse);
	}

	if (compressor_relay != nullptr)
	{
		compressor_relay->Set(compressor_on ? Relay::kForward : Relay::kOff);
	}
}
