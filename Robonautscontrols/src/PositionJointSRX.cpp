/*******************************************************************************
 *
 * File: PositionJointSRX.cpp
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
#include "RobonautsLibrary/MacroStepFactory.h"

#include "RobonautsControls/PositionJointSRX.h"

#ifdef _2018
#include "smartdashboard/SmartDashboard.h" //WPI
#else
#include "frc/smartdashboard/SmartDashboard.h" //WPI
#endif



using namespace tinyxml2;
using namespace frc;

/*******************************************************************************	
 * 
 * Create an instance of a motor control, feedback, and PID 
 * and connect them to the specified inputs
 * clo
 * @param	xml			the XML used to configure this object, see the class
 * 						definition for the XML format
 * 						
 * @param	period		the time (in seconds) between calls to update()
 * @param	priority	the priority at which this thread/task should run
 * 
 ******************************************************************************/
PositionJointSRX::PositionJointSRX(std::string control_name, XMLElement* xml)
	: PeriodicControl(control_name)
{
	Advisory::pinfo("========================= Creating PJS [%s] =========================",
	            control_name.c_str());
	motor = nullptr;

	increment_step = 0.0;
	decrement_step = 0.0;
	delta_position = 0.01;

	m_is_ready = false;
	motor_scale = 1.0; // if invert = true, scale = -1.0, else scale = 1.0

    m_max_velocity = 0.0;
    m_desired_acceleration = 0.0;
    m_desired_deceleration = 0.0;

	raw_position = -999.99;
	raw_velocity = -999;
	actual_position = -999.99;
    actual_velocity = -999.99;
	target_power = 0.0;
	target_position = 0.0;
	command_power = 0.0;
	max_power_delta = 0.2;
	num_setpoints = 0;

    //
	// Register Macro Steps
	//
	new MacroStepProxy<MSPosJointSRXPBCSetPower>(control_name, "SetPower", this);


	for (uint8_t idx = 0; idx < NUM_MOMENTARIES; idx++)
	{
		momentary_power[idx] = 0.0;
		momentary_step[idx] = 0.0;
	}

	setpoint_index = 0;
	for (uint8_t i = 0; i < NUM_SETPOINTS; i++)
	{
		m_pjs_setpoint[i] = 0.0;
		m_setpoint_name[i] = "";
	}

	m_closed_loop = false;
	target_power = 0.0;

	pjs_log = new DataLogger("/robot/logs/pjs", "pjs", "csv", 10, true);


	//
	// Register Macro Steps
	//
//	new MacroStepProxy<MSMotorCLPosSetPosition>(control_name, "SetPosition", this);

	//
	// Parse the Controls XML
	//
	XMLElement *comp;
	const char *name;

	xml->QueryBoolAttribute("closed_loop", &m_closed_loop);
				Advisory::pinfo("===================== closed loop -- %d", m_closed_loop);

	xml->QueryFloatAttribute("max_velocity", &m_max_velocity);
	xml->QueryFloatAttribute("desired_acceleration", &m_desired_acceleration);
	xml->QueryFloatAttribute("desired_deceleration", &m_desired_deceleration);

	comp = xml->FirstChildElement("setpoints");
	if (comp != nullptr)
	{
    	XMLElement *setpoint_comp;
		setpoint_comp = comp->FirstChildElement("setpoint");
 		while (setpoint_comp!=nullptr)
		{
			int setpoint_index = -1;
			setpoint_comp->QueryIntAttribute("index", &setpoint_index);
			if (setpoint_index >= 0 && setpoint_index < PositionJointSRX::NUM_SETPOINTS)
			{
				name = setpoint_comp->Attribute("name");
				if (name != nullptr)
				{
					m_setpoint_name[setpoint_index] = std::string(name);
					Advisory::pinfo("AHHHHHHHHHHHHHHHHHHHHHHH");
				}
				else
				{
					m_setpoint_name[setpoint_index] = std::string("setpoint_") + std::to_string(setpoint_index);
					Advisory::pwarning("%s found unnamed setpoint, using default", getName());
				}

				setpoint_comp->QueryFloatAttribute("pjs", &m_pjs_setpoint[setpoint_index]);

				Advisory::pinfo("%s  -- setpoint %2d: %20s   pjs=%7.2f",getName(),
					setpoint_index, m_setpoint_name[setpoint_index].c_str(),
					m_pjs_setpoint[setpoint_index]);

					if (num_setpoints < setpoint_index + 1)
					{
						num_setpoints = setpoint_index + 1;
					}
			}
			else
			{
				Advisory::pinfo("%s setpoint with index out of range -- %d", getName(), setpoint_index);
			}
    		setpoint_comp = setpoint_comp->NextSiblingElement("setpoint");
		}
	}

	comp = xml-> FirstChildElement("motor");
	if (comp != NULL)
	{
		Advisory::pinfo("%s  creating speed controller", getName());
		motor_scale = comp->BoolAttribute("invert") ? -1.0f : 1.0f;
		motor = XmlRobotUtil::createRSpeedController(comp);
		motor->SetPosition(0.0); // set initial position to known value
		if (m_closed_loop)
		{
			motor->SetControlMode(RSpeedController::POSITION);
		Advisory::pinfo("------------------------- closed loop default on");
		}
		else
		{
			motor->SetControlMode(RSpeedController::DUTY_CYCLE);
		Advisory::pinfo("%s------------------------- closed loop default off", getName().c_str());
		}
		motor->SetPosition(30.0); // set initial position to known value
	}
	else
	{
		Advisory::pwarning("%s  WARNING: required element \"motor\" not found", getName());
	}

	comp = xml-> FirstChildElement("oi");
	while (comp != NULL)
	{
		name = comp->Attribute("name");
		if (name != NULL)
		{
			if (strcmp(name, "analog") == 0)
			{
				Advisory::pinfo("%s  connecting analog channel", getName());
				OIController::subscribeAnalog(comp, this, CMD_MOVE_ANALOG);
				comp->QueryFloatAttribute("delta_scale", &delta_position);
			}
			else if (strcmp(name, "setpoint_idx") == 0)
			{
				Advisory::pinfo("%s  connecting %s channel", getName(), name);
				OIController::subscribeInt(comp, this, CMD_SETPOINT_IDX);
			}
			else if ((strncmp(name, "setpoint", 8) == 0)
				&& (name[8] >= '0') && (name[8] <= '9'))
			{
				Advisory::pinfo("%s  connecting %s channel", getName(), name);
				OIController::subscribeDigital(comp, this, CMD_SETPOINT_0 + (name[8] - '0'));
			}
			else if ((strncmp(name, "momentary", 9) == 0)
					&& (name[9] >= '0') && (name[9] <= '3'))
			{
				int idx = name[9] - '0';

				Advisory::pinfo("%s  connecting %s channel [idx=%d]", getName(), name, idx);
				comp->QueryFloatAttribute("ol_power", &(momentary_power[idx]));
				comp->QueryFloatAttribute("cl_step", &(momentary_step[idx]));

				Advisory::pinfo("%s  olp = %f, cls = %f", getName(), momentary_power[idx], momentary_step[idx] );

				OIController::subscribeDigital(comp, this, CMD_MOMENTARY_0 + idx);
			}
			else if (strcmp(name, "closed_loop_state") == 0)
			{
				Advisory::pinfo("%s  connecting %s channel", getName(), name);
				OIController::subscribeDigital(comp, this, CMD_CLOSED_LOOP_STATE);
			}

		else
			{
				Advisory::pinfo("%s unknown oi %s", getName(), name);
			}
		}
		else
		{
			Advisory::pinfo("%s unnamed oi", getName());
		}
		comp = comp->NextSiblingElement("oi");
	}
}

/*******************************************************************************	
 * 
 * Release any resources allocated by this object
 * 
 ******************************************************************************/
PositionJointSRX::~PositionJointSRX(void)
{
	if (motor != NULL)
	{
		delete motor;
		motor = NULL;
	}
	
	if (pjs_log == nullptr)
	{
	    delete pjs_log;
	    pjs_log = nullptr;
	}
}

/*******************************************************************************	
 *
 *
 *
 ******************************************************************************/
void PositionJointSRX::controlInit(void)
{
    bool is_ready = true;
	
	if(motor == nullptr)
	{
		Advisory::pwarning("%s Position Joint SRX missing required component -- motor", getName());
		is_ready = false;
	}


    m_traj_profile.setConfiguration(m_max_velocity, m_desired_acceleration, m_desired_deceleration, this->getPeriod());

	m_is_ready = is_ready;
}

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
void PositionJointSRX::updateConfig(void)
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
void PositionJointSRX::setAnalog(int id, float val)
{
	if (id == CMD_MOVE_ANALOG)
	{
		target_power = RobotUtil::limit(-1.0, 1.0, val);
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
void PositionJointSRX::setDigital(int id, bool val)
{
		switch (id)
	{
		case CMD_CLOSED_LOOP_STATE:
		{
			setClosedLoop(val);
			
			//Advisory::pinfo("%s %s(%d, %s)", getName(), __FUNCTION__, id, val?"true":"false");

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
        case CMD_SETPOINT_7:    applySetpoint(val, 7); break;

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
void PositionJointSRX::setInt(int id, int val)
{
	Advisory::pinfo("%s %s(%d, %d)", getName(), __FUNCTION__, id, val);
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
void PositionJointSRX::applyMomentary(bool on, int idx)
{
	if (on)
	{
		target_power = 	momentary_power[idx];
	}
	else
	{
		target_power = 0.0;
	}

	Advisory::pinfo("%s PositionJointSRX::applyMomentary(on=%d, idx=%d) p=%f", getName(), on, idx, target_power);
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::applySetpoint(bool on, int idx)
{
	if (on && (idx >= 0) && (idx < num_setpoints))
	{
		setpoint_index = idx;
		target_position = m_pjs_setpoint[idx];
	    Advisory::pinfo("%s applySetpoint: on=%d, idx=%d, targ=%f", getName().c_str(), on, idx, target_position);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::setPosition(float val)
{
	target_position = val;
}

/*******************************************************************************
 *
 ******************************************************************************/
float PositionJointSRX::getPostion(void)
{
	return actual_position;
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::disabledInit()
{
    pjs_log->close();
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::autonomousInit()
{
		Advisory::pinfo("%s PJS", getName());
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::teleopInit()
{
    PositionJointSRX::initLogFile();
	Advisory::pinfo("%s PJS", getName());
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::testInit()
{
}

/**********************************************************************
 *
 * This method is used to initialize the log files, called from
 * teleop init and auton init methods
 *
 **********************************************************************/
void PositionJointSRX::initLogFile(void)
{
    pjs_log->openSegment();

    pjs_log->log("%s, %s, %s, %s, %s, %s, ",
                    "current_time",
                    "raw_position", "actual_position", "target_position",
                    "raw_velocity", "actual_velocity");

    pjs_log->log("%s, %s",
        "traj_position",
        "traj_velocity");

    pjs_log->log("\n");
    pjs_log->flush();

    /*m_traj_profile.m_target_position, m_traj_profile.m_desired_position, m_traj_profile.m_current_postion,
                    m_traj_profile.m_desired_velocity,
                    raw_position, raw_velocity, actual_position, actual_velocity);*/
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	SmartDashboard::PutBoolean(getName() +" closed loop: ", m_closed_loop);


	SmartDashboard::PutNumber(getName() +" raw_position: ", raw_position);
	SmartDashboard::PutNumber(getName() +" actual velocity: ", actual_velocity);
	SmartDashboard::PutNumber(getName() +" actual position: ", actual_position);
	SmartDashboard::PutNumber(getName() +" target position: ", target_position);
	SmartDashboard::PutNumber(getName() +" command position: ", commanded_position);

	SmartDashboard::PutNumber(getName() +" target power: ", target_power);
	SmartDashboard::PutNumber(getName() +" command power: ", command_power);
	SmartDashboard::PutNumber(getName() +" set point idx: ", setpoint_index);
}

/*******************************************************************************
 *
 ******************************************************************************/
void PositionJointSRX::setClosedLoop(bool closed)
{

if (m_closed_loop != closed)
	{
		m_closed_loop = closed;

		if (m_closed_loop)
		{
		    Advisory::pinfo("setting pjs closed loop mode");
            motor->SetControlMode(RSpeedController::POSITION);
		}
		else
		{
            Advisory::pinfo("setting pjs open loop mode");
            motor->SetControlMode(RSpeedController::DUTY_CYCLE);
		}
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
bool PositionJointSRX::isClosedLoop(void)
{
	return m_closed_loop;
}

/*******************************************************************************
 *
 * This method will be called once a period to do anything that is needed,
 * in this case it just sets the motor power to the current value.
 * 
 ******************************************************************************/
void PositionJointSRX::doPeriodic()
{
//is_ready still need to figure out
	
	if(m_is_ready == false)
	{
		Advisory::pinfo("DoPeriodic is not ready");
		pjs_log->log("DoPeriodic is not ready");
		return ;
	}

	double current_time = gsi::Time::getTime();

	// for debugging
	raw_position = motor->GetRawPosition();
	raw_velocity = motor->GetRawSpeed();
	actual_position = motor->GetPosition();
	actual_velocity = motor->GetSpeed();

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

	if (m_closed_loop)
	{
		//Advisory::pinfo("target position = %f", target_position); 
		m_traj_profile.setTargetPosition(target_position);
		commanded_position = m_traj_profile.update();
		motor->Set(commanded_position);
	}
	else
	{
		//
		// Ramp the power changes
		//
		if (command_power + max_power_delta < target_power)
		{
			command_power += max_power_delta;
		}
		else if (command_power - max_power_delta > target_power)
		{
			command_power -= max_power_delta;
		}
		else
		{
			command_power = target_power;
		}
        motor->Set(command_power * motor_scale);
		//
		// Make sure the pjs isn't at a limit switch
		//
	//	Advisory::pinfo("uwu %f", command_power);

	}

	pjs_log->log("%6.8f, %6.8f, %6.8f, %6.8f, %6.8f, %6.8f, ",
	                current_time,
	                raw_position, actual_position, target_position,
	                raw_velocity, actual_velocity);


    pjs_log->log("%6.8f, %6.8f\n",
        m_traj_profile.getTrajectoryPosition(),
        m_traj_profile.getTrajectoryVelocity());
}

/*******************************************************************************
 *
 ******************************************************************************/
MSPosJointSRXPBCSetPower::MSPosJointSRXPBCSetPower(std::string type, tinyxml2::XMLElement *xml, void *control)
	: MacroStepSequence(type, xml, control)
{
	m_parent_control = (PositionJointSRX *)control;

	xml->QueryFloatAttribute("power", &m_power);
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSPosJointSRXPBCSetPower::init(void)
{
	m_parent_control->setAnalog(PositionJointSRX::CMD_MOVE_ANALOG, m_power);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MSPosJointSRXPBCSetPower::update(void)
{
	return next_step;
}
