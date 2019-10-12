/*******************************************************************************
 *
 * File: MotorControl.cpp
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
#include "RobonautsControls/MotorControl.h"
#include "RobonautsLibrary/MacroStepFactory.h"

#ifdef _2018
#include "smartdashboard/SmartDashboard.h" //WPI
#else
#include "frc/smartdashboard/SmartDashboard.h" //WPI
#endif


using namespace tinyxml2;
using namespace frc;

/*******************************************************************************	
 * 
 * Create an instance of a motor control and connect it to the specified
 * motor and inputs
 * 
 ******************************************************************************/
MotorControl::MotorControl(std::string control_name, XMLElement* xml)
	: PeriodicControl(control_name)
{
	XMLElement *comp;
	Advisory::pinfo("========================= Creating Motor Control [%s] =========================",
	            control_name.c_str());
	motor_a = nullptr;
	motor_b = nullptr;

	m_upper_limit_sw = nullptr;
	m_lower_limit_sw = nullptr;

	m_upper_limit_pressed = false;
	m_lower_limit_pressed = false;

	motor_min_control = -1.0;
	motor_max_control = 1.0;

    motor_a_max_current = 100.0;
    motor_b_max_current = 100.0;

	motor_increment_step = 0.1;
	motor_decrement_step = 0.1;

	motor_momentary_a_value = 0.0;
	motor_momentary_b_value = 0.0;
	motor_momentary_c_value = 0.0;
	motor_momentary_d_value = 0.0;

	motor_max_cmd_delta = 0.25;

	motor_target_power = 0.0;
	motor_command_power = 0.0;

	const char *name = nullptr;

	//
	// Register Macro Steps
	//
	new MacroStepProxy<MSMotorPBCSetPower>(control_name, "SetPower", this);

	//
	// Parse XML
	//
    xml->QueryFloatAttribute("min_control", &motor_min_control);
    xml->QueryFloatAttribute("max_control", &motor_max_control);
    xml->QueryFloatAttribute("max_cmd_delta", &motor_max_cmd_delta);
    Advisory::pinfo("  max cmd delta = %f", motor_max_cmd_delta);


	comp = xml-> FirstChildElement("motor");
	if (comp != nullptr)
	{
		Advisory::pinfo("  creating speed controller for motor_a");
		motor_a = XmlRobotUtil::createRSpeedController(comp);
	    comp->QueryUnsignedAttribute("max_current", &motor_a_max_current);
	}
	
	comp = comp-> NextSiblingElement("motor");
	if (comp != nullptr)
	{
		Advisory::pinfo("  creating speed controller for motor_b");
		motor_b = XmlRobotUtil::createRSpeedController(comp);
        comp->QueryUnsignedAttribute("max_current", &motor_b_max_current);
	}
	
	comp = xml->FirstChildElement("digital_input");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "upper_limit") == 0)
			{
				Advisory::pinfo("  creating digital input for %s", name);
				m_upper_limit_sw = XmlRobotUtil::createDigitalInput(comp);
			}
			else if (strcmp(name, "lower_limit") == 0)
			{
				Advisory::pinfo("  creating digital input for %s", name);
				m_lower_limit_sw = XmlRobotUtil::createDigitalInput(comp);
			}
			else
			{
				Advisory::pwarning("  unrecognized digital_input tag for %s", name);
			}
		}
		else
		{
			Advisory::pwarning("  found digital_input tag with no name attribute");
		}

		comp = comp->NextSiblingElement("digital_input");
	}


	comp = xml-> FirstChildElement("oi");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "analog") == 0)
			{
				Advisory::pinfo("  connecting analog channel");
				OIController::subscribeAnalog(comp, this, CMD_ANALOG);
			}
			else if (strcmp(name, "increment") == 0)
			{
				Advisory::pinfo("  connecting increment channel");
				comp->QueryFloatAttribute("step", &motor_increment_step);
				OIController::subscribeDigital(comp, this, CMD_INCREMENT);
			}
			else if (strcmp(name, "decrement") == 0)
			{
				Advisory::pinfo("  connecting decrement channel");
				comp->QueryFloatAttribute("step", &motor_decrement_step);
				OIController::subscribeDigital(comp, this, CMD_DECREMENT);
			}
			else if (strcmp(name, "stop") == 0)
			{
				Advisory::pinfo("  connecting stop channel");
				OIController::subscribeDigital(comp, this, CMD_STOP);
			}
			else if (strcmp(name, "momentary_a") == 0)
			{
				Advisory::pinfo("  connecting momentary_a channel");
				comp->QueryFloatAttribute("value", &motor_momentary_a_value);
				OIController::subscribeDigital(comp, this, CMD_MOMENTARY_A);
			}
			else if (strcmp(name, "momentary_b") == 0)
			{
				Advisory::pinfo("  connecting momentary_b channel");
				comp->QueryFloatAttribute("value", &motor_momentary_b_value);
				OIController::subscribeDigital(comp, this, CMD_MOMENTARY_B);
			}
			else if (strcmp(name, "momentary_c") == 0)
			{
				Advisory::pinfo("  connecting momentary_c channel");
				comp->QueryFloatAttribute("value", &motor_momentary_c_value);
				OIController::subscribeDigital(comp, this, CMD_MOMENTARY_C);
			}
			else if (strcmp(name, "momentary_d") == 0)
			{
				Advisory::pinfo("  connecting momentary_d channel");
				comp->QueryFloatAttribute("value", &motor_momentary_d_value);
				OIController::subscribeDigital(comp, this, CMD_MOMENTARY_D);
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
MotorControl::~MotorControl(void)
{
	if (motor_a != nullptr)
	{
		delete motor_a;
		motor_a = nullptr;
	}
	if (motor_b != nullptr)
	{
		delete motor_b;
		motor_b = nullptr;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MotorControl::controlInit(void)
{
    if ((motor_a != nullptr) && (motor_a_max_current < 100.0))
    {
        motor_a->SetCurrentLimit(motor_a_max_current);
        motor_a->SetCurrentLimitEnabled(true);
    }

    if ((motor_b != nullptr) && (motor_b_max_current < 100.0))
    {
        motor_b->SetCurrentLimit(motor_b_max_current);
        motor_b->SetCurrentLimitEnabled(true);
    }
}

/*******************************************************************************
 *
 ******************************************************************************/
void MotorControl::updateConfig(void)
{

}

/*******************************************************************************	
 *
 * Reset power to 0
 * 
 ******************************************************************************/
void MotorControl::disabledInit(void)
{
	motor_target_power = 0.0;
	motor_command_power = 0.0;
}

/*******************************************************************************	
 *
 * Reset power to 0
 * 
 ******************************************************************************/
void MotorControl::autonomousInit(void)
{
	motor_target_power = 0.0;
	motor_command_power = 0.0;
}

/*******************************************************************************	
 *
 * Reset power to 0
 * 
 ******************************************************************************/
void MotorControl::teleopInit(void)
{
	motor_target_power = 0.0;
	motor_command_power = 0.0;
}

/*******************************************************************************	
 *
 * Reset power to 0
 *
 ******************************************************************************/
void MotorControl::testInit(void)
{
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
void MotorControl::setAnalog(int id, float val)
{
	if (id == CMD_ANALOG)
	{
		motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, val);
	}
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
void MotorControl::setDigital(int id, bool val)
{
	switch (id)
	{
		case CMD_INCREMENT:
		{
			if (val)
			{
				motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, motor_target_power + motor_increment_step);
			}
		} break;
			
		case CMD_DECREMENT:
		{
			if (val)
			{
				motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, motor_target_power - motor_decrement_step);
			}
		} break;
			
		case CMD_STOP:
		{
			if (val)
			{
				motor_target_power = 0.0;
			}
		} break;

		case CMD_MOMENTARY_A:
		{
			if(val)
			{
				motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, motor_momentary_a_value);
			}
			else
			{
				motor_target_power = 0.0;
			}
		} break;

		case CMD_MOMENTARY_B:
		{
			if(val)
			{
				motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, motor_momentary_b_value);
			}
			else
			{
				motor_target_power = 0.0;
			}
		} break;

		case CMD_MOMENTARY_C:
		{
			if(val)
			{
				motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, motor_momentary_c_value);
			}
			else
			{
				motor_target_power = 0.0;
			}
		} break;

		case CMD_MOMENTARY_D:
		{
			if(val)
			{
				motor_target_power = RobotUtil::limit(motor_min_control, motor_max_control, motor_momentary_d_value);
			}
			else
			{
				motor_target_power = 0.0;
			}
		} break;
			
		default:
			break;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MotorControl::setInt(int id, int val)
{
}


/*******************************************************************************	
 *
 ******************************************************************************/
void MotorControl::publish()
{
    // This is obviously intended to be temporary
    SmartDashboard::PutNumber("cur_a_check", motor_a->GetOutputCurrent());

    SmartDashboard::PutBoolean(getName() + " upper_limit", m_upper_limit_pressed);
    SmartDashboard::PutBoolean(getName() + " lower_limit", m_lower_limit_pressed);
}

/*******************************************************************************
 *
 * This method will be called once a period to do anything that is needed,
 * in this case it just sets the motor power to the current value.
 * 
 ******************************************************************************/
void MotorControl::doPeriodic()
{
	// get sensor inputs for this object
	if (m_upper_limit_sw != nullptr)
	{
		m_upper_limit_pressed = m_upper_limit_sw->get();
	}

	if (m_lower_limit_sw != nullptr)
	{
		m_lower_limit_pressed = m_lower_limit_sw->get();
	}

	//
	// process values
	//
	if (motor_target_power > motor_command_power + motor_max_cmd_delta)
	{
		motor_command_power += motor_max_cmd_delta;
	}
	else if (motor_target_power < motor_command_power - motor_max_cmd_delta)
	{
		motor_command_power -= motor_max_cmd_delta;
	}
	else
	{
		motor_command_power = motor_target_power;
	}

	if (m_upper_limit_pressed == true)
	{
		motor_command_power = RobotUtil::limit(motor_min_control, 0.0, motor_command_power);
	}

	if (m_lower_limit_pressed == true)
	{
		motor_command_power = RobotUtil::limit(0.0, motor_max_control, motor_command_power);
	}

	//
	// Set Outputs
	//
	if (motor_a != nullptr)
	{
		motor_a->Set(motor_command_power);
	}

	if (motor_b != nullptr)
	{
		motor_b->Set(motor_command_power);
	}
}


// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 ******************************************************************************/
MSMotorPBCSetPower::MSMotorPBCSetPower(std::string type, tinyxml2::XMLElement *xml, void *control)
	: MacroStepSequence(type, xml, control)
{
	m_parent_control = (MotorControl *)control;

	xml->QueryFloatAttribute("power", &m_power);
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSMotorPBCSetPower::init(void)
{
	m_parent_control->setAnalog(MotorControl::CMD_ANALOG, m_power);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MSMotorPBCSetPower::update(void)
{
	return next_step;
}

