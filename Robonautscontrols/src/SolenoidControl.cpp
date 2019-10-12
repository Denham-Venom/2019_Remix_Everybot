/*******************************************************************************
 *
 * File: SolenoidControl.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RobotUtil.h"
#include "RobonautsLibrary/XmlRobotUtil.h"
#include "gsu/Advisory.h"
#ifdef _2018
#include "smartDashboard/SmartDashboard.h" //WPI
#else
#include "frc/smartDashboard/SmartDashboard.h" //WPI
#endif
#include "RobonautsControls/SolenoidControl.h"

#include "RobonautsLibrary/MacroStepFactory.h"

using namespace tinyxml2;

/*******************************************************************************	
 * 
 * Create an instance of this object and configure it based on the provided
 * XML, period, and priority
 * 
 * @param	xml			the XML used to configure this object, see the class
 * 						definition for the XML format
 * 						
 * @param	period		the time (in seconds) between calls to update()
 * @param	priority	the priority at which this thread/task should run
 * 
 ******************************************************************************/
SolenoidControl::SolenoidControl(std::string control_name, XMLElement* xml)
	: PeriodicControl(control_name)
{
	XMLElement *comp;
	Advisory::pinfo("========================= Creating Solenoid Control [%s] =========================",
	            control_name.c_str());
	solenoid_a = nullptr;
	solenoid_b = nullptr;
	solenoid_state = false;
	
	//
	// Register Macro Steps
	//
	new MacroStepProxy<MSSolenoidPBCSetState>(control_name, "SetState", this);

	//
	// Parse the XML
	//
	comp = xml->FirstChildElement("solenoid");
	if (comp != nullptr)
	{
		solenoid_a = XmlRobotUtil::createSolenoid(comp);
	}
	
	comp = comp->NextSiblingElement("solenoid");
	if (comp != nullptr)
	{
		solenoid_b = XmlRobotUtil::createSolenoid(comp);
	}
	
	const char *name;
	comp = xml->FirstChildElement("oi");
	while (comp != NULL)
	{
		name = comp->Attribute("name");
		if (name != NULL)
		{
			if (strcmp(name, "on") == 0)
			{
				Advisory::pinfo("  connecting to on channel");
				OIController::subscribeDigital(comp, this, CMD_ON);
			}
			else if (strcmp(name, "off") == 0)
			{
				Advisory::pinfo("  connecting to off channel");
				OIController::subscribeDigital(comp, this, CMD_OFF);
			}
			else if (strcmp(name, "toggle") == 0)
			{
				Advisory::pinfo("  connecting to toggle channel");
				OIController::subscribeDigital(comp, this, CMD_TOGGLE);
			}
			else if (strcmp(name, "state") == 0)
			{
				Advisory::pinfo("  connecting to state channel");
				OIController::subscribeDigital(comp, this, CMD_STATE);
			}
		}
		comp = comp->NextSiblingElement("oi");
	}
}

/*******************************************************************************	
 *
 * Release any resources used by this object
 * 
 ******************************************************************************/
SolenoidControl::~SolenoidControl(void)
{
	if (solenoid_a != nullptr)
	{
		delete solenoid_a;
		solenoid_a = nullptr;
	}

	if (solenoid_b != nullptr)
	{
		delete solenoid_b;
		solenoid_b = nullptr;
	}
}

/*******************************************************************************	
 *
 *
 *
 ******************************************************************************/
void SolenoidControl::controlInit(void)
{

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
void SolenoidControl::updateConfig(void)
{

}

/*******************************************************************************
 *
 * Sets the state of the solenoid based on the command id and value
 * 
 * @param	id	the command id that indicates what the val argument means
 * @param	val	the new value
 * 
 ******************************************************************************/
void SolenoidControl::setDigital(int id, bool val)
{
	switch (id)
	{
		case CMD_ON:
			if (val) solenoid_state = true;
			break;

		case CMD_OFF:
			if (val) solenoid_state = false;
			break;

		case CMD_TOGGLE:
			if (val) solenoid_state = !solenoid_state;
			break;

		case CMD_STATE:
			solenoid_state = val;
			break;

		default:
			break;
	}
}

/*******************************************************************************	
 *
 * 
 * 
 ******************************************************************************/
void SolenoidControl::setAnalog(int id, float val)
{
}

/*******************************************************************************	
 *
 * 
 *
 ******************************************************************************/
void SolenoidControl::setInt(int id, int val)
{
}

/*******************************************************************************	
 *
 * 
 *
 ******************************************************************************/
void SolenoidControl::disabledInit(void)
{
	solenoid_state = false;
}

/*******************************************************************************	
 *
 * State to false for start of auton
 *
 ******************************************************************************/
void SolenoidControl::autonomousInit(void)
{
	solenoid_state = false;
}

/*******************************************************************************
 *
 * Set state to false for start of teleop
 * 
 ******************************************************************************/
void SolenoidControl::teleopInit(void)
{
	solenoid_state = false;
}

/*******************************************************************************	
 *
 * Set state to false for start of test
 *
 ******************************************************************************/
void SolenoidControl::testInit(void)
{
	solenoid_state = false;
}

/*******************************************************************************
 *
 * Sends data to dashboard
 *
 ******************************************************************************/
void SolenoidControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	SmartDashboard::PutBoolean(getName() +" state: ", solenoid_state);
}

void SolenoidControl::setState(bool state)
{
	solenoid_state = state;
}

/*******************************************************************************
 *
 * Sets solenoid to a boolean value each period
 *
 ******************************************************************************/
void SolenoidControl::doPeriodic()
{
	if (solenoid_a != nullptr)
	{
		solenoid_a->Set(solenoid_state);
	}

	if (solenoid_b != nullptr)
	{
		solenoid_b->Set( ! solenoid_state);
	}
}

MSSolenoidPBCSetState::MSSolenoidPBCSetState(std::string type, tinyxml2::XMLElement *xml, void *control) :
		MacroStepSequence(type, xml, control)
{
	m_parent_control = (SolenoidControl *) control;
	m_state = true;
	xml->QueryBoolAttribute("state", &m_state);
}

void MSSolenoidPBCSetState::init(void)
{
	m_parent_control->setState(m_state);
}
MacroStep * MSSolenoidPBCSetState::update(void)
{
	return next_step;
}

