/*******************************************************************************
 *
 * File: KiwiDriveControl.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#ifdef _2018
#include "smartDashboard/SmartDashboard.h"
#include "SpeedController.h"
#include "Solenoid.h"
#else
#include "frc/smartDashboard/SmartDashboard.h"
#include "frc/SpeedController.h"
#include "frc/Solenoid.h"
#endif

#include "gsu/Advisory.h"

#include "RobonautsLibrary/RobotUtil.h"
#include "RobonautsLibrary/XmlRobotUtil.h"
#include "RobonautsLibrary/MacroStepFactory.h"

#include "RobonautsControls/KiwiDriveControl.h"

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
 * *****************************************************************************/
KiwiDriveControl::KiwiDriveControl(std::string control_name, XMLElement* xml)
	:	PeriodicControl(control_name)
{
    Advisory::pinfo("========================= Creating Kiwi Drive Control [%s] =========================",
                control_name.c_str());
	XMLElement *comp;

	const char *name;

    for (int m = 0; m < NUM_MOTORS; m++)
    {
        m_drive_motor[m] = nullptr;
        m_motor_cmd[m] = 0.0;
        m_motor_trg[m] = 0.0;
    }

	for(int i = 0; i < NUM_INPUTS; i++)
	{
	    m_input_cmd[i] = 0.0;
	}
	
	name = xml->Attribute("name");
	if (name == nullptr)
	{
		name="drive";
		Advisory::pcaution(  "WARNING: KiwiArchade created without name, assuming \"%s\"", name);
	}

	//
	// Register Macro Steps
	//
	new MacroStepProxy<MSKiwiDrivePower>(control_name, "DrivePower", this);
	
	//
	// Parse the XML
	//
	comp = xml-> FirstChildElement("motor");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "front_left") == 0)
			{				
				Advisory::pinfo("  creating speed controller for %s", name);
				m_drive_motor[MOTOR_FRONT_LEFT] = XmlRobotUtil::createRSpeedController(comp);
			}
			else if (strcmp(name, "front_right") == 0)
			{
			    Advisory::pinfo("  creating speed controller for %s", name);
                m_drive_motor[MOTOR_FRONT_RIGHT] = XmlRobotUtil::createRSpeedController(comp);
			}
			else if (strcmp(name, "back") == 0)
			{
			    Advisory::pinfo("  creating speed controller for %s", name);
                m_drive_motor[MOTOR_BACK] = XmlRobotUtil::createRSpeedController(comp);
			}
			else
			{
				Advisory::pinfo("  could not create unknown motor for %s", name);
			}
		}
		comp = comp->NextSiblingElement("motor");
	}

	comp = xml-> FirstChildElement("oi");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
            Advisory::pinfo("  connecting oi: \"%s\"", name);
			if (strcmp(name, "forward") == 0)
			{
				OIController::subscribeAnalog(comp, this, CMD_FORWARD);
			}
			else if (strcmp(name, "lateral") == 0)
			{
				OIController::subscribeAnalog(comp, this, CMD_LATERAL);
			}
			else if (strcmp(name, "rotate") == 0)
			{
                OIController::subscribeAnalog(comp, this, CMD_ROTATE);
			}
			else
			{
				Advisory::pwarning("  could not connect unknown oi: name=\"%s\"", name);
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
KiwiDriveControl::~KiwiDriveControl(void)
{
    for (int m = 0; m < NUM_MOTORS; m++)
    {
        if (m_drive_motor[m] != nullptr)
        {
            delete m_drive_motor[m];
            m_drive_motor[m] = nullptr;
        }

        m_motor_cmd[m] = 0.0;
        m_motor_trg[m] = 0.0;
    }
}

/*******************************************************************************	
 *
 ******************************************************************************/
void KiwiDriveControl::controlInit()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        m_input_cmd[i] = 0.0;
    }
}

/*******************************************************************************	
 *
 ******************************************************************************/
void KiwiDriveControl::disabledInit()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        m_input_cmd[i] = 0.0;
    }
}

/*******************************************************************************	
 *
 ******************************************************************************/
void KiwiDriveControl::autonomousInit()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        m_input_cmd[i] = 0.0;
    }
}

/*******************************************************************************	
 *
 ******************************************************************************/
void KiwiDriveControl::teleopInit()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        m_input_cmd[i] = 0.0;
    }
}

/*******************************************************************************	
 *
 ******************************************************************************/
void KiwiDriveControl::testInit()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        m_input_cmd[i] = 0.0;
    }
}

/*******************************************************************************
 *
 ******************************************************************************/
void KiwiDriveControl::setInt(int id, int val)
{
}

/*******************************************************************************
 *
 * Sets the state of the control based on the command id and value
 * 
 * Handled command ids are CMD_TURN and CMD_FORWARD, all others are ignored
 * 
 * @param	id	the command id that indicates what the val argument means
 * @param	val	the new value
 * 
 ******************************************************************************/
void KiwiDriveControl::setAnalog(int id, float val)
{
	switch (id)
	{
        case CMD_FORWARD:
        {
            m_input_cmd[INPUT_FORWARD] = val;
        } break;

        case CMD_LATERAL:
        {
            m_input_cmd[INPUT_LATERAL] = val;
        } break;

		case CMD_ROTATE:
		{
		    m_input_cmd[INPUT_ROTATE] = val;
		} break;

		default:
			break;
	}
}

/*******************************************************************************	
 *
 * Sets the state of the control based on the command id and value
 *  
 * Handled command ids are CMD_BRAKE_[ON|OFF|TOGGLE|STATE] and 
 * CMD_GEAR_[HIGH|LOW|TOGGLE|STATE], all others are ignored
 *
 * @param	id	the command id that indicates what the val argument means
 * @param	val	the new value
 * 
 ******************************************************************************/
void KiwiDriveControl::setDigital(int id, bool val)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void KiwiDriveControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

    SmartDashboard::PutNumber(getName() +" forward: ", m_input_cmd[INPUT_FORWARD]);
    SmartDashboard::PutNumber(getName() +" lateral: ", m_input_cmd[INPUT_LATERAL]);
    SmartDashboard::PutNumber(getName() +" rotate: ",  m_input_cmd[INPUT_ROTATE]);

    SmartDashboard::PutNumber(getName() +" mtr_cmd_fl: ",  m_motor_cmd[0]);
    SmartDashboard::PutNumber(getName() +" mtr_trg_fl: ",  m_motor_trg[0]);
    SmartDashboard::PutNumber(getName() +" mtr_cmd_fr: ",  m_motor_cmd[1]);
    SmartDashboard::PutNumber(getName() +" mtr_trg_fr: ",  m_motor_trg[1]);
    SmartDashboard::PutNumber(getName() +" mtr_cmd_b: ",  m_motor_cmd[2]);
    SmartDashboard::PutNumber(getName() +" mtr_trg_b: ",  m_motor_trg[2]);
}

#define SIN_30 (0.500000)
#define COS_30 (0.866025)

/*******************************************************************************	
 * 
 * Sets the actuator values every period
 * 
 ******************************************************************************/
void KiwiDriveControl::doPeriodic()
{
	//
	// Read Sensors
	//

	//
	// Calculate Values
	//
    m_motor_trg[MOTOR_FRONT_LEFT]  = (-SIN_30 * m_input_cmd[INPUT_LATERAL]) - (COS_30 * m_input_cmd[INPUT_FORWARD]) + m_input_cmd[INPUT_ROTATE];
    m_motor_trg[MOTOR_FRONT_RIGHT] = (-SIN_30 * m_input_cmd[INPUT_LATERAL]) + (COS_30 * m_input_cmd[INPUT_FORWARD]) + m_input_cmd[INPUT_ROTATE];
    m_motor_trg[MOTOR_BACK]        = m_input_cmd[INPUT_LATERAL] + m_input_cmd[INPUT_ROTATE];

    m_motor_cmd[MOTOR_FRONT_LEFT] = m_motor_trg[MOTOR_FRONT_LEFT];
    m_motor_cmd[MOTOR_FRONT_RIGHT] = m_motor_trg[MOTOR_FRONT_RIGHT];
    m_motor_cmd[MOTOR_BACK] = m_motor_trg[MOTOR_BACK];

    //
    // Normalize the Motor Commands
    //
    float max_cmd_magnitude = 0.0f;
    for(int m = 0; m < NUM_MOTORS; m++)
    {
        max_cmd_magnitude = std::max(max_cmd_magnitude, (float)fabs(m_motor_cmd[m]));
    }

    if (max_cmd_magnitude > 1.0)
    {
        for(int m = 0; m < NUM_MOTORS; m++)
        {
            m_motor_cmd[m] = m_motor_cmd[m]/max_cmd_magnitude;
        }
    }

	//
	// Set Outputs
	//
    for(int m = 0; m < NUM_MOTORS; m++)
    {
        if (m_drive_motor[m] != nullptr)
        {
            m_drive_motor[m]->Set(m_motor_cmd[m]);
        }
    }
}


// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 ******************************************************************************/
MSKiwiDrivePower::MSKiwiDrivePower(std::string type, tinyxml2::XMLElement *xml, void *control)
	: MacroStepSequence(type, xml, control)
{
	parent_control = (KiwiDriveControl *)control;

    m_forward = 0.0;
    m_lateral = 0.0;
    m_rotate =  0.0;

    xml->QueryFloatAttribute("forward", &m_forward);
    xml->QueryFloatAttribute("lateral", &m_lateral);
    xml->QueryFloatAttribute("rotate", &m_rotate);
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSKiwiDrivePower::init(void)
{
	parent_control->setAnalog(KiwiDriveControl::CMD_FORWARD, m_forward);
	parent_control->setAnalog(KiwiDriveControl::CMD_LATERAL, m_lateral);
	parent_control->setAnalog(KiwiDriveControl::CMD_ROTATE,  m_rotate);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MSKiwiDrivePower::update(void)
{
	return next_step;
}
