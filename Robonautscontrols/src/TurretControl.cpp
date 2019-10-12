/*******************************************************************************
 *
 * File: TurretControl.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <math.h>

//#include "WPILib.h"

#include "RobonautsLibrary/RobotUtil.h"
#include "RobonautsLibrary/XmlRobotUtil.h"
#include "gsu/Advisory.h"
#include "gsi/Time.h"
#include "RobonautsLibrary/DataLogger.h"

#include "RobonautsControls/TurretControl.h"

#ifdef _2018
#include "smartdashboard/SmartDashboard.h" //WPI
#else
#include "frc/smartdashboard/SmartDashboard.h" //WPI
#endif



using namespace tinyxml2;
using namespace frc;

/*******************************************************************************	
 * 
 * Create an instance of a motor control, feedback pot, and PID 
 * and connect them to the specified inputs
 * clo
 * @param	xml			the XML used to configure this object, see the class
 * 						definition for the XML format
 * 						
 * @param	period		the time (in seconds) between calls to update()
 * @param	priority	the priority at which this thread/task should run
 * 
 ******************************************************************************/
TurretControl::TurretControl(std::string control_name, XMLElement* xml)
	: PeriodicControl(control_name)
{
	XMLElement *comp;
	Advisory::pinfo("========================= Creating Turrent Control [%s] =========================",
	            control_name.c_str());
	motor = nullptr;
	pot = nullptr;
	aps = nullptr;
	pid = nullptr;
	upper_limit_sw = nullptr;
	lower_limit_sw = nullptr;

	increment_step = 0.0;
	decrement_step = 0.0;
	delta_position = 0.01;

	motor_scale = 1.0; // if invert = true, scale = -1.0, else scale = 1.0
	upper_limit_invert = false;
	lower_limit_invert = false;
	
	upper_limit_pressed = false;
	lower_limit_pressed = false;

    m_max_velocity = 0.0;
    m_desired_acceleration = 0.0;
    m_desired_deceleration = 0.0;

	pot_ready = false;
	raw_position = -999.99;
	raw_velocity = -999;
	actual_position = -999.99;
    actual_velocity = -999.99;
	target_power = 0.0;
	target_position = 0.0;
	command_power = 0.0;
	max_power_delta = 0.2;
	num_setpoints = 0;

	for (uint8_t idx = 0; idx < NUM_MOMENTARIES; idx++)
	{
		momentary_power[idx] = 0.0;
		momentary_step[idx] = 0.0;
	}

	setpoint_index = 0;
	for (uint8_t i = 0; i < NUM_SETPOINTS; i++)
	{
		setpoint[i] = 0.0;
	}


	closed_loop = false;
	target_power = 0.0;
	target_position = 0.0;

	turret_log = new DataLogger("/robot/logs/turret", "turret", "csv", 10, true);

	const char *name;

	xml->QueryBoolAttribute("closed_loop", &closed_loop);

	if (xml->QueryFloatAttribute("setpoint0", &setpoint[0]) != XML_NO_ATTRIBUTE) num_setpoints = 1;
	if (xml->QueryFloatAttribute("setpoint1", &setpoint[1]) != XML_NO_ATTRIBUTE) num_setpoints = 2;
	if (xml->QueryFloatAttribute("setpoint2", &setpoint[2]) != XML_NO_ATTRIBUTE) num_setpoints = 3;
	if (xml->QueryFloatAttribute("setpoint3", &setpoint[3]) != XML_NO_ATTRIBUTE) num_setpoints = 4;
	if (xml->QueryFloatAttribute("setpoint4", &setpoint[4]) != XML_NO_ATTRIBUTE) num_setpoints = 5;
	if (xml->QueryFloatAttribute("setpoint5", &setpoint[5]) != XML_NO_ATTRIBUTE) num_setpoints = 6;
	if (xml->QueryFloatAttribute("setpoint6", &setpoint[6]) != XML_NO_ATTRIBUTE) num_setpoints = 7;
	if (xml->QueryFloatAttribute("setpoint7", &setpoint[7]) != XML_NO_ATTRIBUTE) num_setpoints = 8;
	if (xml->QueryFloatAttribute("setpoint8", &setpoint[8]) != XML_NO_ATTRIBUTE) num_setpoints = 9;
	if (xml->QueryFloatAttribute("setpoint9", &setpoint[9]) != XML_NO_ATTRIBUTE) num_setpoints = 10;


	xml->QueryFloatAttribute("max_velocity", &m_max_velocity);
	xml->QueryFloatAttribute("desired_acceleration", &m_desired_acceleration);
	xml->QueryFloatAttribute("desired_deceleration", &m_desired_deceleration);

	//
	// Register Macro Steps
	//
//	new MacroStepProxy<MSMotorCLPosSetPosition>(control_name, "SetPosition", this);

	//
	// Parse the Controls XML
	//
	comp = xml-> FirstChildElement("motor");
	if (comp != NULL)
	{
		Advisory::pinfo("  creating speed controller");
		motor = XmlRobotUtil::createRSpeedController(comp);
		motor_scale = comp->BoolAttribute("invert") ? -1.0f : 1.0f;
	}
	else
	{
		Advisory::pwarning("  WARNING: required element \"motor\" not found");
	}

	if ((closed_loop == true) && (motor != nullptr))
	{
		Advisory::pinfo("setting turret closed loop mode");
		motor->SetControlMode(RSpeedController::POSITION);
//		motor->SetControlMode(RSpeedController::TRAPEZOID);
	}

	comp = xml-> FirstChildElement("pot");
	if (comp != NULL)
	{
		Advisory::pinfo("  creating pot");
		pot = XmlRobotUtil::createPot(comp);
	}
	
	comp = xml-> FirstChildElement("aps");
	if (comp != NULL)
	{
		Advisory::pinfo("  creating aps");
		aps = XmlRobotUtil::createAbsPosSensor(comp);
	}

	comp = xml-> FirstChildElement("pid");
	if (comp != NULL)
	{
		Advisory::pinfo("  creating PID");
		pid = XmlRobotUtil::createPID(comp);
	}

	comp = xml-> FirstChildElement("digital_input");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "upper_limit") == 0)
			{
				Advisory::pinfo("  connecting upper limit switch");
				upper_limit_sw = XmlRobotUtil::createDigitalInput(comp);
				comp->QueryBoolAttribute("invert", &upper_limit_invert);
			}
			else if (strcmp(name, "lower_limit") == 0)
			{
				Advisory::pinfo("  connecting lower limit switch");
				lower_limit_sw = XmlRobotUtil::createDigitalInput(comp);
				comp->QueryBoolAttribute("invert", &lower_limit_invert);
			}
		}
		comp = comp->NextSiblingElement("digital_input");
	}
	
	comp = xml-> FirstChildElement("oi");
	while (comp != NULL)
	{
		name = comp->Attribute("name");
		if (name != NULL)
		{
			if (strcmp(name, "analog") == 0)
			{
				Advisory::pinfo("  connecting analog channel");
				OIController::subscribeAnalog(comp, this, CMD_MOVE_ANALOG);
				comp->QueryFloatAttribute("delta_scale", &delta_position);
			}
			else if (strcmp(name, "setpoint_idx") == 0)
			{
				Advisory::pinfo("  connecting %s channel", name);
				OIController::subscribeInt(comp, this, CMD_SETPOINT_IDX);
			}
			else if ((strncmp(name, "setpoint", 8) == 0)
				&& (name[8] >= '0') && (name[8] <= '9'))
			{
				Advisory::pinfo("  connecting %s channel", name);
				OIController::subscribeDigital(comp, this, CMD_SETPOINT_0 + (name[8] - '0'));
			}
			else if ((strncmp(name, "momentary", 9) == 0)
					&& (name[9] >= '0') && (name[9] <= '3'))
			{
				int idx = name[9] - '0';

				Advisory::pinfo("  connecting %s channel [idx=%d]", name, idx);
				comp->QueryFloatAttribute("ol_power", &(momentary_power[idx]));
				comp->QueryFloatAttribute("cl_step", &(momentary_step[idx]));

				Advisory::pinfo("  olp = %f, cls = %f", momentary_power[idx], momentary_step[idx] );

				OIController::subscribeDigital(comp, this, CMD_MOMENTARY_0 + idx);
			}
			else if (strcmp(name, "closed_loop_state") == 0)
			{
				Advisory::pinfo("  connecting %s channel", name);
				OIController::subscribeDigital(comp, this, CMD_CLOSED_LOOP_STATE);
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
TurretControl::~TurretControl(void)
{
	if (motor != NULL)
	{
		delete motor;
		motor = NULL;
	}
	
	if (pot != NULL)
	{
		delete pot;
		pot = NULL;
	}
	
	if (aps != NULL)
	{
		delete aps;
		aps = NULL;
	}

	if (pid != NULL)
	{
		delete pid;
		pid = NULL;
	}

	if (turret_log == nullptr)
	{
	    delete turret_log;
	    turret_log = nullptr;
	}
}

/*******************************************************************************	
 *
 *
 *
 ******************************************************************************/
void TurretControl::controlInit(void)
{
   m_traj_profile.setConfiguration(m_max_velocity, m_desired_acceleration, m_desired_deceleration, this->getPeriod());

	if ((pot == nullptr) && (aps == nullptr))
	{
		Advisory::pwarning("  WARNING: element \"pot\" and \"aps\" not found, cannot use closed loop");
	}

	if (pid == nullptr)
	{
		Advisory::pwarning("  WARNING: element \"pid\" not found, cannot use closed loop");
	}
}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
void TurretControl::updateConfig(void)
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
void TurretControl::setAnalog(int id, float val)
{
	Advisory::pinfo("%s(%d, %f)", __FUNCTION__, id, val);
	if (id == CMD_MOVE_ANALOG)
	{
		target_power = RobotUtil::limit(-1.0, 1.0, val);

		if (pid != nullptr)
		{
			target_position += (val * delta_position);
		}
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
void TurretControl::setDigital(int id, bool val)
{
	Advisory::pinfo("%s(%d, %s)", __FUNCTION__, id, val?"true":"false");
	switch (id)
	{
		case CMD_CLOSED_LOOP_STATE:
		{
			closed_loop = val;
		} break;

		case CMD_MOMENTARY_0:	applyMomentary(val, 0);  break;
		case CMD_MOMENTARY_1:	applyMomentary(val, 1);  break;
		case CMD_MOMENTARY_2:	applyMomentary(val, 2);  break;
		case CMD_MOMENTARY_3:	applyMomentary(val, 3);  break;

		case CMD_SETPOINT_0: 	applySetpoint(val, 0); break;
		case CMD_SETPOINT_1: 	applySetpoint(val, 1); break;
		case CMD_SETPOINT_2: 	applySetpoint(val, 2); break;
		case CMD_SETPOINT_3: 	applySetpoint(val, 3); break;
		case CMD_SETPOINT_4: 	applySetpoint(val, 4); break;
		case CMD_SETPOINT_5: 	applySetpoint(val, 5); break;
		case CMD_SETPOINT_6: 	applySetpoint(val, 6); break;
		case CMD_SETPOINT_7: 	applySetpoint(val, 7); break;
		case CMD_SETPOINT_8: 	applySetpoint(val, 8); break;
		case CMD_SETPOINT_9: 	applySetpoint(val, 9); break;

		case CMD_INCREMENT_POS:
		{
			if (setpoint_index < NUM_SETPOINTS - 1)
			{
				applySetpoint(val, setpoint_index + 1);
			}
			else
			{
				applySetpoint(val, NUM_SETPOINTS - 1);
			}
		} break;

		case CMD_DECREMENT_POS:
		{
			if (setpoint_index > 0)
			{
				applySetpoint(val, setpoint_index - 1);
			}
			else
			{
				applySetpoint(val, 0);
			}
		} break;

		default:
			break;
	}
}

/*******************************************************************************	
 *
 * This is the callback for OIController::subscribeInt, if the XML config
 * specifies an int, the constructor of this object will connect that
 * input to this method.
 *
 * @param id	the control id passed to the subscribe
 * @param val	the new value of the int channel that was subscribed to
 *
 ******************************************************************************/
void TurretControl::setInt(int id, int val)
{
	Advisory::pinfo("%s(%d, %d)", __FUNCTION__, id, val);
	if (id == CMD_SETPOINT_IDX)
	{
		if (val >= 0)
		{
			applySetpoint( true, val/45);
		}
		else
		{
			applySetpoint( false, val);
		}
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::applyMomentary(bool on, int idx)
{
	if (on)
	{
		target_power = 	momentary_power[idx];

		if (pot != nullptr)
		{
			target_position = pid->limitTarget(target_position + momentary_step[idx]);

			// @TODO: if value change passes a setpoint, then adjust the setpoint index
		}
	}
	else
	{
		target_power = 0.0;
	}

	Advisory::pinfo("TurretControl::applyMomentary(on=%d, idx=%d) p=%f", on, idx, target_power);
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::applySetpoint(bool on, int idx)
{
	if (on && (idx >= 0) && (idx < num_setpoints))
	{
		setpoint_index = idx;
		target_position = setpoint[idx];
	}

	Advisory::pinfo("applySetpoint: on=%d, idx=%d, targ=%f\n", on, idx, target_position);
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::setPosition(float val)
{
	target_position = val;
}

/*******************************************************************************
 *
 ******************************************************************************/
float TurretControl::getPostion(void)
{
	return actual_position;
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::disabledInit()
{
    turret_log->close();
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::autonomousInit()
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::teleopInit()
{
    TurretControl::initLogFile();
}

/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::testInit()
{
}

/**********************************************************************
 *
 * This method is used to initialize the log files, called from
 * teleop init and auton init methods
 *
 **********************************************************************/
void TurretControl::initLogFile(void)
{
   turret_log->openSegment();

    turret_log->log("%s, %s, %s, %s, %s, %s, ",
                    "current_time",
                    "raw_position", "actual_position", "target_position",
                    "raw_velocity", "actual_velocity");

    turret_log->log("%s, %s",
        "traj_position",
        "traj_velocity");

    turret_log->log("\n");
    turret_log->flush();



    /*m_traj_profile.m_target_position, m_traj_profile.m_desired_position, m_traj_profile.m_current_postion,
                    m_traj_profile.m_desired_velocity,
                    raw_position, raw_velocity, actual_position, actual_velocity);*/
}
/*******************************************************************************
 *
 ******************************************************************************/
void TurretControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	SmartDashboard::PutBoolean(getName() +" closed loop: ", closed_loop);

	SmartDashboard::PutBoolean(getName() +" upper limit: ", upper_limit_pressed);
	SmartDashboard::PutBoolean(getName() +" lower limit: ", lower_limit_pressed);

	SmartDashboard::PutBoolean(getName() +" pot ready: ", pot_ready);

	SmartDashboard::PutNumber(getName() +" raw_position: ", raw_position);
	SmartDashboard::PutNumber(getName() +" actual velocity: ", actual_velocity);
	SmartDashboard::PutNumber(getName() +" actual position: ", actual_position);
	SmartDashboard::PutNumber(getName() +" target position: ", target_position);

	if (pid != nullptr)
	{
		SmartDashboard::PutNumber(getName() +" PID Ep: ", pid->getErrorP());
		SmartDashboard::PutNumber(getName() +" PID Ei: ", pid->getErrorI());
		SmartDashboard::PutNumber(getName() +" PID Ed: ", pid->getErrorD());
	}

	SmartDashboard::PutNumber(getName() +" target power: ", target_power);
	SmartDashboard::PutNumber(getName() +" command power: ", command_power);
	SmartDashboard::PutNumber(getName() +" set point idx: ", setpoint_index);
}

/*******************************************************************************
 *
 ******************************************************************************/
bool TurretControl::isClosedLoop(void)
{
	return closed_loop && pot_ready;
}

/*******************************************************************************
 *
 * This method will be called once a period to do anything that is needed,
 * in this case it just sets the motor power to the current value.
 * 
 ******************************************************************************/
void TurretControl::doPeriodic()
{
	// for debugging
	raw_position = motor->GetRawPosition();
	raw_velocity = motor->GetRawSpeed();
	actual_position = motor->GetPosition();
	actual_velocity = motor->GetSpeed();
	double current_time = gsi::Time::getTime();

	SmartDashboard::PutNumber(getName() +" raw_velocity: ", raw_velocity);

	//
	// Make sure the motor doesn't jump when enabled
	//
	if (getPhase() == DISABLED)
	{
		target_position = actual_position;
		target_power = 0.0;
		command_power = 0.0;

		m_traj_profile.setInitialPosition(target_position);
	}

//	if ((motor->GetControlMode() == RSpeedController::TRAPEZOID)
//		|| (motor->GetControlMode() == RSpeedController::POSITION))
//	{

		m_traj_profile.setTargetPosition(target_position);
		motor->Set(m_traj_profile.update());
//	}
//	else
//	{
//		Advisory::pinfo("TurretControl open loop, setting %f", target_position);
//		//
//		// Ramp the power changes
//		//
//		if (command_power + max_power_delta < target_power)
//		{
//			command_power += max_power_delta;
//		}
//		else if (command_power - max_power_delta > target_power)
//		{
//			command_power -= max_power_delta;
//		}
//		else
//		{
//			command_power = target_power;
//		}
//
//		//
//		// Make sure the lift isn't at a limit switch
//		//
//		if (upper_limit_pressed)
//		{
//			command_power = RobotUtil::limit(-1.0, 0.0, command_power);
//		}
//
//		if (lower_limit_pressed)
//		{
//			command_power = RobotUtil::limit(0.0, 1.0, command_power);
//		}
//
//		//
//		// Set output power
//		//
//		if (motor != NULL)
//		{
//			motor->Set(command_power * motor_scale);
//		}
//	}

	turret_log->log("%6.8f, %6.8f, %6.8f, %6.8f, %6.8f, %6.8f, ",
	                current_time,
	                raw_position, actual_position, target_position,
	                raw_velocity, actual_velocity);


    turret_log->log("%6.8f, %6.8f\n",
        m_traj_profile.getTrajectoryPosition(),
        m_traj_profile.getTrajectoryVelocity());
}

//
//
//MSMotorCLPosSetPosition::MSMotorCLPosSetPosition(std::string type, tinyxml2::XMLElement *xml, void *control)
//	: MacroStepSequence(type, xml, control)
//{
//	motor_control = (MotorCLPos *)control;  // @TODO: should change so dynamic_cast can be used
//	if (motor_control == nullptr)
//	{
//		Advisory::pinfo("ERROR: MSMotorCLPosSetPosition set to incompatible control");
//	}
//
//	position = xml->FloatAttribute("position");
//	tolerance = xml->FloatAttribute("tolerance");
//}
//
//void MSMotorCLPosSetPosition::init(void)
//{
//	motor_control->setPosition(position);
//}
//
//MacroStep * MSMotorCLPosSetPosition::update(void)
//{
//	if (! motor_control->isClosedLoop())
//	{
//		parent_macro->abort();
//		return nullptr;
//	}
//
//	if (fabs(motor_control->getPostion() - position) < tolerance)
//	{
//		return next_step;
//	}
//
//	return this;
//}
//
