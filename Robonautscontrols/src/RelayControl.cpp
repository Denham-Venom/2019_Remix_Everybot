/*******************************************************************************
 *
 * File: RelayControl.cpp
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
#include "RobonautsControls/RelayControl.h"
#include "RobonautsLibrary/MacroStepFactory.h"

using namespace tinyxml2;
using namespace std;
using namespace frc;

/*******************************************************************************	
 * 
 * Create an instance of a motor control and connect it to the specified
 * motor and inputs
 * 
 ******************************************************************************/
RelayControl::RelayControl(std::string control_name, XMLElement* xml)
    : PeriodicControl(control_name)
{
	relay = nullptr;
	relay_value = Relay::kOff;

	const char *name;
	XMLElement *comp;


	//
	// Register Macro Steps
	//
	new MacroStepProxy<MSRelaySetState>(control_name, "SetRelay", this);

	Advisory::pinfo("========================= Creating Relay Control [%s] =========================",
	            control_name.c_str());
	comp = xml-> FirstChildElement("relay");
	if (comp != nullptr)
	{
		Advisory::pinfo("  creating relay");
		relay = XmlRobotUtil::createRelay(comp);
	}
	
	comp = xml-> FirstChildElement("oi");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "on") == 0)
			{
				Advisory::pinfo("  connecting on channel");
				OIController::subscribeDigital(comp, this, CMD_ON);
			} 
			else if (strcmp(name, "off") == 0)
			{
				Advisory::pinfo("  connecting off channel");
				OIController::subscribeDigital(comp, this, CMD_OFF);
			} 
			else if (strcmp(name, "forward") == 0)
			{
				Advisory::pinfo("  connecting forward channel");
				OIController::subscribeDigital(comp, this, CMD_FORWARD);
			} 
			else if (strcmp(name, "reverse") == 0)
			{
				Advisory::pinfo("  connecting reverse channel");
				OIController::subscribeDigital(comp, this, CMD_REVERSE);
			}
		}

		comp = comp->NextSiblingElement("oi");
	}
}

/*******************************************************************************	
 * 
 * Release any resources allocated by this object
 * 
 ******************************************************************************/
RelayControl::~RelayControl(void)
{
	if (relay != nullptr)
	{
		delete relay;
		relay = nullptr;
	}
}

/*******************************************************************************	
 *
 * 
 *
 ******************************************************************************/
void RelayControl::controlInit(void)
{

}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
void RelayControl::updateConfig(void)
{

}

/*******************************************************************************
 *
 * turn the relay off
 *
 ******************************************************************************/
void RelayControl::disabledInit(void)
{
	relay_value = Relay::kOff;
}

/*******************************************************************************	
 *
 * turn the relay off
 * 
 ******************************************************************************/
void RelayControl::autonomousInit(void)
{
	relay_value = Relay::kOff;
}

/*******************************************************************************	
 *
 * turn the relay off
 * 
 ******************************************************************************/
void RelayControl::teleopInit(void)
{
	relay_value = Relay::kOff;
}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
void RelayControl::testInit(void)
{
	relay_value = Relay::kOff;
}

/*******************************************************************************	
 *
 * This is the callback for OIController::subscribeAnalog, if the XML config 
 * specifies an analog input, the constructor of this object will connect that 
 * input to this method.
 * 
 * @param id	the control id passed to the subscribe
 * @param val	the new value of the analog channel that was subscribed to
 * 
 ******************************************************************************/
void RelayControl::setAnalog(int id, float val)
{
	//Do Nothing
}

/*******************************************************************************	
 *
 * This is the callback for OIController::setDigital, if the XML config 
 * specifies an increment, decrement, or stop input, the constructor 
 * of this object will connect that/those input(s) to this method.
 * 
 * @param id	the control id passed to the subscribe
 * @param val	the new value of the digital channel that was subscribed to
 * 
 ******************************************************************************/
void RelayControl::setDigital(int id, bool val)
{
	switch (id)
	{
		case CMD_ON:
		{
			if (val)
			{
				relay_value = Relay::kOn;
			}
		} break;

		case CMD_OFF:
		{
			if (val)
			{
				relay_value = Relay::kOff;
			}
		} break;

		case CMD_FORWARD:
		{
			if (val)
			{
				relay_value = Relay::kForward;
			}
		} break;

		case CMD_REVERSE:
		{
			if (val)
			{
				relay_value = Relay::kReverse;
			}
		} break;

		default:
		{
			// Do Nothing
		} break;
	}
}

/*******************************************************************************	
 *
 * This is the callback for OIController::subscribeAnalog, if the XML config
 * specifies an analog input, the constructor of this object will connect that
 * input to this method.
 *
 * @param id	the control id passed to the subscribe
 * @param val	the new value of the analog channel that was subscribed to
 * 
 ******************************************************************************/
void RelayControl::setInt(int id, int val)
{
	//Do Nothing
}

/*******************************************************************************	
 * 
 ******************************************************************************/
void RelayControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	switch(relay_value)
	{
		case Relay::kOff:
		{
			SmartDashboard::PutString(getName() + " state: ", string("OFF"));
		} break;

		case Relay::kOn:
		{
			SmartDashboard::PutString(getName() + " state: ", string(" ON"));
		} break;

		case Relay::kForward:
		{
			SmartDashboard::PutString(getName() + " state: ", string("FWD"));
		} break;

		case Relay::kReverse:
		{
			SmartDashboard::PutString(getName() + " state: ", string("REV"));
		} break;
	}
}

/*******************************************************************************
 *
 * This method will be called once a period to do anything that is needed,
 * in this case it just sets the motor power to the current value.
 *
 ******************************************************************************/
void RelayControl::doPeriodic()
{
	if (relay != nullptr)
	{
		relay->Set(relay_value);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
MSRelaySetState::MSRelaySetState(std::string type, tinyxml2::XMLElement *xml, void *control)
	: MacroStepSequence(type, xml, control)
{
	char *state;
	m_parent_control = (RelayControl *)control;
	state = (char *)xml->Attribute("state");
	if (state != NULL)
	{
		if (strcmp(state, "off") == 0)
		{
		    m_cmd_id = RelayControl::CMD_OFF;
		}
		else if (strcmp(state, "on") == 0)
		{
		    m_cmd_id = RelayControl::CMD_ON;
		}
		else if (strcmp(state, "forward") == 0)
		{
		    m_cmd_id = RelayControl::CMD_FORWARD;
		}
		else if (strcmp(state, "reverse") == 0)
		{
		    m_cmd_id = RelayControl::CMD_REVERSE;
		}

	}

	m_relay_value = true;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSRelaySetState::init(void)
{
	m_parent_control->setDigital(m_cmd_id, m_relay_value);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MSRelaySetState::update(void)
{
	return next_step;
}
