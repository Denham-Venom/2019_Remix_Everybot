/*******************************************************************************
 *
 * File: ServoControl.cpp
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
#include "RobonautsControls/ServoControl.h"
#include "RobonautsLibrary/MacroStepFactory.h"

#ifdef _2018
#include "smartDashboard/SmartDashboard.h"
#else
#include "frc/smartDashboard/SmartDashboard.h"
#endif

using namespace tinyxml2;
using namespace frc;

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
ServoControl::ServoControl(std::string control_name, XMLElement* xml)
	: PeriodicControl(control_name)
{
	XMLElement *comp;
	Advisory::pinfo("========================= Creating Servo Control [%s] =========================",
	            control_name.c_str());

	m_servo = nullptr;
	m_servo_target_position = 0.0;
	m_servo_step_size = 5.0;
	m_servo_up_position = 10.0;;
	m_servo_down_position = -10.0;
	m_servo_target_velocity = 0.0;

	//
	// Register Macro Steps
	//
	//new MacroStepProxy<MSServoPBCSetState>(control_name, "SetState", this);

	//
	// Parse the XML
	//
	xml->QueryFloatAttribute("step_size", &m_servo_step_size);
	xml->QueryFloatAttribute("up_position", &m_servo_up_position);
	xml->QueryFloatAttribute("down_position", &m_servo_down_position);

	comp = xml->FirstChildElement("servo");
	if (comp != nullptr)
	{
			Advisory::pinfo("  creating servo");
			m_servo = XmlRobotUtil::createServo(comp);

	}
	m_servo_target_position = m_servo_down_position;
	
	const char *name;
	comp = xml->FirstChildElement("oi");
	while (comp != NULL)
	{
		name = comp->Attribute("name");
		if (name != NULL)
		{
			if (strcmp(name, "servo_up") == 0)
			{
				Advisory::pinfo("  connecting servo up channel");
				OIController::subscribeDigital(comp, this, CMD_UP);
			}
			 else if (strcmp(name, "servo_down") == 0)
            {
			     Advisory::pinfo("  connecting servo down channel");
			     OIController::subscribeDigital(comp, this, CMD_DOWN);
			}
			     else if (strcmp(name, "servo_state") == 0)
			{
			     Advisory::pinfo("  connecting servo state channel");
			     OIController::subscribeDigital(comp, this, CMD_STATE);
			}
			     else if (strcmp(name, "servo_toggle") == 0)
			{
			     Advisory::pinfo("  connecting servo toggle channel");
			     OIController::subscribeDigital(comp, this, CMD_TOGGLE);
			}
            else if (strcmp(name, "servo_velocity") == 0)
			{
			     Advisory::pinfo("  connecting servo velocity channel");
			     OIController::subscribeAnalog(comp, this, CMD_VELOCITY);
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
ServoControl::~ServoControl(void)
{
	if (m_servo != nullptr)
	{
		delete m_servo;
		m_servo = nullptr;
	}

}

/*******************************************************************************	
 *
 *
 *
 ******************************************************************************/
void ServoControl::controlInit(void)
{

	if (m_servo == nullptr)
	{
		Advisory::pwarning("  WARNING: Shooter element \"servo_servo\" not found");

	}
}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
void ServoControl::updateConfig(void)
{

}

/*******************************************************************************
 *
 * Sets the state of the Servo based on the command id and value
 * 
 * @param	id	the command id that indicates what the val argument means
 * @param	val	the new value
 * 
 ******************************************************************************/
void ServoControl::setDigital(int id, bool val)
{
	switch (id)
	{
		case CMD_UP:
		{
			if(val)
			{
				m_servo_target_position = RobotUtil::limit(m_servo_down_position, m_servo_up_position, m_servo_target_position + m_servo_step_size);
				Advisory::pinfo("ServoControl::setDigital(id=%d, val=%d) step +%f to target of %f", id, val,  m_servo_step_size, m_servo_target_position);
			}
		} break;

		case CMD_DOWN:
		{
			if(val)
			{
				m_servo_target_position = RobotUtil::limit(m_servo_down_position, m_servo_up_position, m_servo_target_position - m_servo_step_size);
				Advisory::pinfo("ServoControl::setDigital(id=%d, val=%d) step -%f to target of %f", id, val,  m_servo_step_size, m_servo_target_position);
			}
		} break;

		case CMD_STATE:
		{
			if(val)
			{
				m_servo_target_position = m_servo_up_position;
			}
			else
			{
				m_servo_target_position = m_servo_down_position;
			}
			Advisory::pinfo("ServoControl::setDigital(id=%d, val=%d) state +%f to target of %f", id, val,  m_servo_step_size, m_servo_target_position);

		} break;

		case CMD_TOGGLE:
		{
			if(val)
			{
				if(m_servo_target_position == m_servo_down_position)
				{
					m_servo_target_position = m_servo_up_position;
				}
				else
				{
					m_servo_target_position = m_servo_down_position;
				}
				Advisory::pinfo("ServoControl::setDigital(id=%d, val=%d) toggle +%f to target of %f", id, val,  m_servo_step_size, m_servo_target_position);
			}
		} break;

	}
}

/*******************************************************************************	
 *
 * 
 * 
 ******************************************************************************/
void ServoControl::setAnalog(int id, float val)
{
	switch(id)
	{
		case CMD_VELOCITY:
		{
			m_servo_target_velocity=val;
			Advisory::pinfo("setting servo to %f", m_servo_target_velocity);
		} break;
	}
}

/*******************************************************************************	
 *
 * 
 *
 ******************************************************************************/
void ServoControl::setInt(int id, int val)
{
}

/*******************************************************************************	
 *
 * 
 *
 ******************************************************************************/
void ServoControl::disabledInit(void)
{

}

/*******************************************************************************	
 *
 * State to false for start of auton
 *
 ******************************************************************************/
void ServoControl::autonomousInit(void)
{

}

/*******************************************************************************
 *
 * Set state to false for start of teleop
 * 
 ******************************************************************************/
void ServoControl::teleopInit(void)
{

}

/*******************************************************************************	
 *
 * Set state to false for start of test
 *
 ******************************************************************************/
void ServoControl::testInit(void)
{

}

/*******************************************************************************
 *
 * Sends data to dashboard
 *
 ******************************************************************************/
void ServoControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	SmartDashboard::PutNumber(getName() + " target position: ", m_servo_target_position);
}

/*******************************************************************************
 *
 * Sets Servo to a boolean value each period
 *
 ******************************************************************************/
void ServoControl::doPeriodic()
{
	m_servo_target_position = RobotUtil::limit(m_servo_up_position, m_servo_down_position,
			m_servo_target_position + m_servo_target_velocity);

	if (m_servo != nullptr)
	{
		m_servo->SetAngle(m_servo_target_position);
	}
}

