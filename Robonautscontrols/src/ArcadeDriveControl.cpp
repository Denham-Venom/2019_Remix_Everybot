/*******************************************************************************
 *
 * File: ArcadeDriveControl.cpp
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
#include "gsi/Time.h"

#ifdef _2018
#include "smartDashboard/SmartDashboard.h" //WPI
#else
#include "frc/smartDashboard/SmartDashboard.h" //WPI
#endif

#include "RobonautsControls/ArcadeDriveControl.h"
#include "RobonautsLibrary/MacroStepFactory.h"

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
ArcadeDriveControl::ArcadeDriveControl(std::string control_name, XMLElement* xml)
	:	PeriodicControl(control_name)
{
    Advisory::pinfo("========================= Creating Arcade Drive Control [%s] =========================",
                control_name.c_str());
	XMLElement *comp;

	const char *name;

    m_data_logger = new DataLogger("/robot/logs", "drive", "csv", 50, true);

	fl_drive_motor = nullptr;
	fr_drive_motor = nullptr;
	bl_drive_motor = nullptr;
	br_drive_motor = nullptr;

    m_accelerometer = nullptr;
    m_accelerometer_x = 0.0;
    m_accelerometer_y = 0.0;
    m_accelerometer_z = 0.0;

    m_imu = nullptr;
    m_imu_accel_x = 0.0;
    m_imu_accel_y = 0.0;
    m_imu_accel_z = 0.0;

    m_imu_mag_x = 0.0;
    m_imu_mag_y = 0.0;
    m_imu_mag_z = 0.0;

    m_imu_rate_x = 0.0;
    m_imu_rate_y = 0.0;
    m_imu_rate_z = 0.0;

    m_imu_angle_x = 0.0;
    m_imu_angle_y = 0.0;
    m_imu_angle_z = 0.0;

    m_imu_roll  = 0.0;
    m_imu_pitch = 0.0;
    m_imu_yaw   = 0.0;

    m_imu_quat_w = 0.0;
    m_imu_quat_x = 0.0;
    m_imu_quat_y = 0.0;
    m_imu_quat_z = 0.0;

    m_imu_bar_press   = 0.0;
    m_imu_temperature = 0.0;

	gear_solenoid = nullptr;
	brake_solenoid = nullptr;

	brake_engaged = false;
	brake_solenoid_invert = false;

	gear_high = false;
	gear_solenoid_invert = false;

	strafe_enabled = false;
	strafe_state = false;

	fl_drive_motor_cmd = 0.0;
	fr_drive_motor_cmd = 0.0;
	bl_drive_motor_cmd = 0.0;
	br_drive_motor_cmd = 0.0;

	trn_power = 0.0;
	fwd_power = 0.0;
	arc_turn_power = -0.5;
	arc_fwd_power = 0.2;
	arc_turn_scale = 0.0;
	m_low_power_scale = 0.5;
	m_low_power_active = false;
	
	arc_input = 0;
	arc_power = false;
	
	name = xml->Attribute("name");
	if (name == nullptr)
	{
		name="drive";
		Advisory::pcaution(  "WARNING: DriveArchade created without name, assuming \"%s\"", name);
	}

	//
	// Register Macro Steps
	//
	new MacroStepProxy<MSDriveArcadeDrivePower>(control_name, "DrivePower", this);
	xml->QueryFloatAttribute("low_power_scale", &m_low_power_scale);
	
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
				fl_drive_motor = XmlRobotUtil::createRSpeedController(comp);
				//fl_drive_motor_invert = comp->BoolAttribute("invert") ? -1.0 : 1.0;
			}
			else if (strcmp(name, "front_right") == 0)
			{
			    Advisory::pinfo("  creating speed controller for %s", name);
				fr_drive_motor = XmlRobotUtil::createRSpeedController(comp);
				//fr_drive_motor_invert = comp->BoolAttribute("invert") ? -1.0 : 1.0;
			}
			else if (strcmp(name, "back_left") == 0)
			{
			    Advisory::pinfo("  creating speed controller for %s", name);
				bl_drive_motor = XmlRobotUtil::createRSpeedController(comp);
				//bl_drive_motor_invert = comp->BoolAttribute("invert") ? -1.0 : 1.0;
			}
			else if (strcmp(name, "back_right") == 0)
			{
				Advisory::pinfo("  creating speed controller for %s", name);
				br_drive_motor = XmlRobotUtil::createRSpeedController(comp);
				//br_drive_motor_invert = comp->BoolAttribute("invert") ? -1.0 : 1.0;
			}
		}
		comp = comp->NextSiblingElement("motor");
	}

	comp = xml-> FirstChildElement("solenoid");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "brake") == 0)
			{				
				Advisory::pinfo("  creating solenoid for %s", name);
				brake_solenoid = XmlRobotUtil::createSolenoid(comp);
				brake_solenoid_invert = comp->BoolAttribute("invert");
			}
			else if (strcmp(name, "gear") == 0)
			{
				Advisory::pinfo("  creating solenoid for %s", name);
				gear_solenoid = XmlRobotUtil::createSolenoid(comp);
				gear_solenoid_invert = comp->BoolAttribute("invert");
			}
		}
		comp = comp->NextSiblingElement("solenoid");
	}

    comp = xml-> FirstChildElement("accelerometer");
    if (comp != nullptr)
    {
        // if type built_in then
        Advisory::pinfo("  creating accelerometer");
        m_accelerometer = new BuiltInAccelerometer();
    }

    comp = xml-> FirstChildElement("imu");
    if (comp != nullptr)
    {
        // if type ADIS16448 then
        Advisory::pinfo("  creating imu");
        m_imu = new ADIS16448_IMU();
    }

    comp = xml-> FirstChildElement("oi");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "forward") == 0)
			{
				Advisory::pinfo("  connecting to forward channel");
				OIController::subscribeAnalog(comp, this, CMD_FORWARD);
			}
			else if (strcmp(name, "turn") == 0)
			{
				Advisory::pinfo("  connecting to turn channel");
				OIController::subscribeAnalog(comp, this, CMD_TURN);
			}
			else if (strcmp(name, "low_power_on") == 0)
			{
				Advisory::pinfo("  connecting to low_power on channel");
				OIController::subscribeDigital(comp, this, CMD_LOW_POWER_ON);
			}
			else if (strcmp(name, "low_power_off") == 0)
			{
				Advisory::pinfo("  connecting to low_power off channel");
				OIController::subscribeDigital(comp, this, CMD_LOW_POWER_OFF);
			}
			else if (strcmp(name, "low_power_toggle") == 0)
			{
				Advisory::pinfo("  connecting to low_power toggle channel");
				OIController::subscribeDigital(comp, this, CMD_LOW_POWER_TOGGLE);
			}
			else if (strcmp(name, "low_power_state") == 0)
			{
				Advisory::pinfo("  connecting to low_power state channel");
				OIController::subscribeDigital(comp, this, CMD_LOW_POWER_STATE);
			}
			else if (strcmp(name, "strafe_on") == 0)
			{
				Advisory::pinfo("  connecting to strafe on channel");
				OIController::subscribeDigital(comp, this, CMD_STRAFE_ON);
				strafe_enabled = true;
			}
			else if (strcmp(name, "strafe_off") == 0)
			{
				Advisory::pinfo("  connecting to strafe off channel");
				OIController::subscribeDigital(comp, this, CMD_STRAFE_OFF);
				strafe_enabled = true;
			}
			else if (strcmp(name, "strafe_toggle") == 0)
			{
				Advisory::pinfo("  connecting to strafe toggle channel");
				OIController::subscribeDigital(comp, this, CMD_STRAFE_TOGGLE);
				strafe_enabled = true;
			}
			else if (strcmp(name, "strafe_state") == 0)
			{
				Advisory::pinfo("  connecting to strafe state channel");
				OIController::subscribeDigital(comp, this, CMD_STRAFE_STATE);
				strafe_enabled = true;
			}
			else if (strcmp(name, "brake_on") == 0)
			{
				Advisory::pinfo("  connecting to brake_on channel");
				OIController::subscribeDigital(comp, this, CMD_BRAKE_ON);
			}
			else if (strcmp(name, "brake_off") == 0)
			{
				Advisory::pinfo("  connecting to brake_off channel");
				OIController::subscribeDigital(comp, this, CMD_BRAKE_OFF);
			}
			else if (strcmp(name, "brake_toggle") == 0)
			{
				Advisory::pinfo("  connecting to brake_toggle channel");
				OIController::subscribeDigital(comp, this, CMD_BRAKE_TOGGLE);
			}
			else if (strcmp(name, "brake_state") == 0)
			{
				Advisory::pinfo("  connecting to brake_state channel");
				OIController::subscribeDigital(comp, this, CMD_BRAKE_STATE);
			}
			else if (strcmp(name, "gear_high") == 0)
			{
				Advisory::pinfo("  connecting to gear_high channel");
				OIController::subscribeDigital(comp, this, CMD_GEAR_HIGH);
			}
			else if (strcmp(name, "gear_low") == 0)
			{
				Advisory::pinfo("  connecting to gear_low channel");
				OIController::subscribeDigital(comp, this, CMD_GEAR_LOW);
			}
			else if (strcmp(name, "gear_toggle") == 0)
			{
				Advisory::pinfo("  connecting to gear_toggle channel");
				OIController::subscribeDigital(comp, this, CMD_GEAR_TOGGLE);
			}
			else if (strcmp(name, "gear_state") == 0)
			{
				Advisory::pinfo("  connecting to gear_state channel");
				OIController::subscribeDigital(comp, this, CMD_GEAR_STATE);
			}
			else if (strcmp(name, "arc") == 0)
			{
				Advisory::pinfo("  connecting to arc channel");
				OIController::subscribeInt(comp, this, CMD_ARC);
			}
			else if (strcmp(name, "power_arc") == 0)
			{
				Advisory::pinfo("  connecting to power_arc channel");
				OIController::subscribeDigital(comp, this, CMD_POWER_ARC);
				comp->QueryFloatAttribute("arc_turn_power", &arc_turn_power);
				comp->QueryFloatAttribute("arc_fwd_power", &arc_fwd_power);
				comp->QueryFloatAttribute("arc_turn_scale", &arc_turn_scale);
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
ArcadeDriveControl::~ArcadeDriveControl(void)
{
	if (fl_drive_motor == nullptr)
	{
		delete fl_drive_motor;
		fl_drive_motor = nullptr;
	}
	
	if (fr_drive_motor == nullptr)
	{
		delete fr_drive_motor;
		fr_drive_motor = nullptr;
	}
	
	if (bl_drive_motor == nullptr)
	{
		delete bl_drive_motor;
		bl_drive_motor = nullptr;
	}
	
	if (br_drive_motor == nullptr)
	{
		delete br_drive_motor;
		br_drive_motor = nullptr;
	}

	if (gear_solenoid != nullptr)
	{
		delete gear_solenoid;
		gear_solenoid = nullptr;
	}
	
	if (brake_solenoid != nullptr)
	{
		delete brake_solenoid;
		brake_solenoid = nullptr;
	}
}

/*******************************************************************************	
 *
 ******************************************************************************/
void ArcadeDriveControl::updateConfig()
{
}

/*******************************************************************************	
 *
 ******************************************************************************/
void ArcadeDriveControl::controlInit()
{
	fwd_power = 0.0;
	trn_power = 0.0;
}

/*******************************************************************************	
 *
 ******************************************************************************/
void ArcadeDriveControl::disabledInit()
{
    m_data_logger->close();

	fwd_power = 0.0;
	trn_power = 0.0;
}

/*******************************************************************************	
 *
 ******************************************************************************/
void ArcadeDriveControl::autonomousInit()
{
    logFileInit("Auton");

    fwd_power = 0.0;
	trn_power = 0.0;
}

/*******************************************************************************	
 *
 ******************************************************************************/
void ArcadeDriveControl::teleopInit()
{
    logFileInit("Teleop");

    fwd_power = 0.0;
	trn_power = 0.0;
}

/*******************************************************************************	
 *
 ******************************************************************************/
void ArcadeDriveControl::testInit()
{
    logFileInit("Test");

    fwd_power = 0.0;
	trn_power = 0.0;
}

/*******************************************************************************
 *
 ******************************************************************************/

void ArcadeDriveControl::setInt(int id, int val)
{
     try
     {
         switch (id)
         {
             case CMD_ARC:
             {
                 if (val == 90)
                 {
                	 trn_power = 0.1;
                	 fwd_power = 0.2;
                 }
                 else if (val == 270)
                 {
                	 trn_power = 0.1;
                	 fwd_power = -0.2;
                 }
                 else
                 {
                	;//Do nothing
                 }

            } break;
        }
    }
    catch (...)
    {
        Advisory::pwarning("EXCEPTION: caught in LightControl::setInt");
    }
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
void ArcadeDriveControl::setAnalog(int id, float val)
{
	switch (id)
	{
		case CMD_TURN:
			if (arc_power == false)
			{
			trn_power = val;
			}
			else if (arc_power == true)
			{
				//if (fwd_power == 0.0)
					//trn_power == 0.0;
				//else
				trn_power = arc_turn_power;
				Advisory::pinfo("ArcadeDriveControl::setAnalog TURN(id: %d, val: %f, tp: %f, atp: %f)",
						id, val, trn_power, arc_turn_power);
			}
			break;

		case CMD_FORWARD:
			if (arc_power == false)
			{
			fwd_power = val;
			}
			else if (arc_power == true)
			{
				// going forward
				if (val>0)
				{
					trn_power = arc_turn_power * arc_turn_scale;
					fwd_power = arc_fwd_power;
					Advisory::pinfo("ArcadeDriveControl::setAnalog  FWD(ID %d, turn %f, fp %f, atp %f)",
							id, trn_power, arc_fwd_power, arc_turn_power);
				}
				// going backward
				else if (val<0)
				{
					trn_power = -(arc_turn_power);
					fwd_power = -(arc_fwd_power);
					Advisory::pinfo("ArcadeDriveControl::setAnalog  BACK(ID %d, turn %f, fp %f, atp %f)",
							id, trn_power, arc_fwd_power, arc_turn_power);
				}
				// stopped
				else if (val == 0)
				{
					fwd_power = 0;//brake
					trn_power = 0;
					Advisory::pinfo("STOP");
				}
			}
			break;

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
void ArcadeDriveControl::setDigital(int id, bool val)
{
	switch(id)
	{
		case CMD_BRAKE_ON:
			if (val) brake_engaged = true;
			break;
			
		case CMD_BRAKE_OFF:
			if (val) brake_engaged = false;
			break;
			
		case CMD_BRAKE_TOGGLE:
			if (val) brake_engaged = !brake_engaged;
			break;
			
		case CMD_BRAKE_STATE:
			brake_engaged = val;
			break;
			
		case CMD_GEAR_HIGH:
			if (val) gear_high = true;
			break;
			
		case CMD_GEAR_LOW:
			if (val) gear_high = false;
			break;
			
		case CMD_GEAR_TOGGLE:
			if (val) gear_high = !gear_high;
			break;
			
		case CMD_GEAR_STATE:
			gear_high = val;
			break;

		case CMD_STRAFE_ON:
			if (val) strafe_state = true;
			break;

		case CMD_STRAFE_OFF:
			if (val) strafe_state = false;
			break;

		case CMD_STRAFE_TOGGLE:
			if (val) strafe_state = !strafe_state;
			break;

		case CMD_STRAFE_STATE:
			strafe_state = val;
			break;

		case CMD_LOW_POWER_ON:
			if (val) m_low_power_active = true;
			break;

		case CMD_LOW_POWER_OFF:
			if (val) m_low_power_active = false;
			break;

		case CMD_LOW_POWER_TOGGLE:
			if (val) m_low_power_active = !m_low_power_active;
			break;

		case CMD_LOW_POWER_STATE:
			m_low_power_active = val;
			break;

		case CMD_POWER_ARC:
			arc_power = val;
			if (arc_power == 0)
			{
				fwd_power = 0;
				trn_power = 0;
				Advisory::pinfo("arc_power off, so FORCING FWD/TRN TO STOP!");
			}
			break;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void ArcadeDriveControl::publish()
{
	SmartDashboard::PutBoolean(std::string("  ") + getName() + "  ", (getCyclesSincePublish() > 0));
	SmartDashboard::PutNumber(getName() +" cycles: ", getCyclesSincePublish());

	SmartDashboard::PutNumber(getName() +" forward: ", fwd_power);
	SmartDashboard::PutNumber(getName() +" turn: ", trn_power);

	if (gear_solenoid != nullptr)
	{
		SmartDashboard::PutBoolean(getName() +" gear: ", gear_high);
	}

	if (brake_solenoid != nullptr)
	{
		SmartDashboard::PutBoolean(getName() +" brake: ", brake_engaged);
	}

	if (fl_drive_motor != nullptr)
	{
		SmartDashboard::PutNumber(getName() +" fl cmd: ", fl_drive_motor_cmd);
	}

	if (fr_drive_motor != nullptr)
	{
		SmartDashboard::PutNumber(getName() +" fr cmd: ", fr_drive_motor_cmd);
	}

	if (bl_drive_motor != nullptr)
	{
		SmartDashboard::PutNumber(getName() +" bl cmd: ", bl_drive_motor_cmd);
	}

	if (br_drive_motor != nullptr)
	{
		SmartDashboard::PutNumber(getName() +" br cmd: ", br_drive_motor_cmd);
	}

	if (strafe_enabled)
	{
		SmartDashboard::PutBoolean(getName() +" strafe: ", strafe_state);
	}

    if (m_accelerometer != nullptr)
    {
        SmartDashboard::PutNumber(getName() + " accel_x", m_accelerometer_x);
        SmartDashboard::PutNumber(getName() + " accel_y", m_accelerometer_y);
        SmartDashboard::PutNumber(getName() + " accel_z", m_accelerometer_z);
    }

    if(m_imu != nullptr)
    {
        SmartDashboard::PutNumber(getName() + " IMU Accel X", m_imu->GetAccelX()); // m_accel_x -- from sensor
        SmartDashboard::PutNumber(getName() + " IMU Accel Y", m_imu->GetAccelY());
        SmartDashboard::PutNumber(getName() + " IMU Accel Z", m_imu->GetAccelZ());

        SmartDashboard::PutNumber(getName() + " IMU Mag X", m_imu->GetMagX());  // m_mag_x  -- from sensor
        SmartDashboard::PutNumber(getName() + " IMU Mag Y", m_imu->GetMagY());
        SmartDashboard::PutNumber(getName() + " IMU Mag Z", m_imu->GetMagZ());

        SmartDashboard::PutNumber(getName() + " IMU Rate X", m_imu->GetRateX()); // m_gyro_x  -- from sensor
        SmartDashboard::PutNumber(getName() + " IMU Rate Y", m_imu->GetRateY());
        SmartDashboard::PutNumber(getName() + " IMU Rate Z", m_imu->GetRateZ());

        SmartDashboard::PutNumber(getName() + " IMU Angle X", m_imu->GetAngleX());  // m_integ_gyro_x, m_integ_gyro_x += (gyro_x - m_gyro_offset_x) * dt;
        SmartDashboard::PutNumber(getName() + " IMU Angle Y", m_imu->GetAngleY());
        SmartDashboard::PutNumber(getName() + " IMU Angle Z", m_imu->GetAngleZ());

        SmartDashboard::PutNumber(getName() + " IMU Roll",  m_imu->GetRoll()); // calculated
        SmartDashboard::PutNumber(getName() + " IMU Pitch", m_imu->GetPitch());
        SmartDashboard::PutNumber(getName() + " IMU Yaw",   m_imu->GetYaw());

        SmartDashboard::PutNumber(getName() + " IMU Quat W", m_imu->GetQuaternionW());  // calculated
        SmartDashboard::PutNumber(getName() + " IMU Quat X", m_imu->GetQuaternionX());
        SmartDashboard::PutNumber(getName() + " IMU Quat Y", m_imu->GetQuaternionY());
        SmartDashboard::PutNumber(getName() + " IMU Quat Z", m_imu->GetQuaternionZ());

        SmartDashboard::PutNumber(getName() + " IMU Bar Press",   m_imu->GetBarometricPressure());
        SmartDashboard::PutNumber(getName() + " IMU Temperature", m_imu->GetTemperature());
    }
}

/*******************************************************************************
 *
 ******************************************************************************/
void ArcadeDriveControl::logFileInit(std::string phase)
{
    Advisory::pinfo("initializing log file for %s", phase.c_str());

    m_data_logger->openSegment();

    m_data_logger->log("%s, %s, %s, ",
        "time", "phase", "pet"
    );

    m_data_logger->log("%s, %s,   %s, %s, %s, %s,   %s, %s, %s, ",
        "trn_power", "fwd_power",
        "fl_drive_motor_cmd", "fr_drive_motor_cmd", "bl_drive_motor_cmd", "br_drive_motor_cmd",
        "brake_engaged", "gear_high", "strafe_state");

    if (m_accelerometer != nullptr)
    {
        m_data_logger->log("%s, %s, %s, ",
            "m_accelerometer_x", "m_accelerometer_y", "m_accelerometer_z");
    }

    if (m_imu!= nullptr)
    {
        m_data_logger->log("%s, %s, %s,   %s, %s, %s,   %s, %s, %s,   %s, %s, %s,   %s, %s, %s,   %s, %s, %s, %s,     %s, %s ",
            "m_imu_accel_x", "m_imu_accel_y", "m_imu_accel_z",
            "m_imu_mag_x", "m_imu_mag_y", "m_imu_mag_z",
            "m_imu_rate_x", "m_imu_rate_y", "m_imu_rate_z",
            "m_imu_angle_x", "m_imu_angle_y", "m_imu_angle_z",
            "m_imu_roll", "m_imu_pitch", "m_imu_yaw",
            "m_imu_quat_w", "m_imu_quat_x", "m_imu_quat_y", "m_imu_quat_z",
            "m_imu_bar_press", "m_imu_temperature");
    }

    m_data_logger->log("\n");

    m_data_logger->flush();
}

/*******************************************************************************
 *
 ******************************************************************************/
void ArcadeDriveControl::logFileAppend(void)
{
    m_data_logger->log("%f, %d, %f, ",
        gsi::Time::getTime(), this->getPhase(), this->getPhaseElapsedTime());

    m_data_logger->log("%f, %f,   %f, %f, %f, %f,   %d, %d, %d, ",
        trn_power, fwd_power,
        fl_drive_motor_cmd, fr_drive_motor_cmd, bl_drive_motor_cmd,br_drive_motor_cmd,
        brake_engaged, gear_high, strafe_state);

    if (m_accelerometer != nullptr)
    {
        m_data_logger->log("%f, %f, %f, ",
            m_accelerometer_x, m_accelerometer_y, m_accelerometer_z);
    }

    if (m_imu!= nullptr)
    {
        m_data_logger->log("%f, %f, %f,   %f, %f, %f,   %f, %f, %f,   %f, %f, %f,   %f, %f, %f,   %f, %f, %f, %f,     %f, %f ",
            m_imu_accel_x, m_imu_accel_y, m_imu_accel_z,
            m_imu_mag_x, m_imu_mag_y, m_imu_mag_z,
            m_imu_rate_x, m_imu_rate_y, m_imu_rate_z,
            m_imu_angle_x, m_imu_angle_y, m_imu_angle_z,
            m_imu_roll, m_imu_pitch, m_imu_yaw,
            m_imu_quat_w, m_imu_quat_x, m_imu_quat_y, m_imu_quat_z,
            m_imu_bar_press, m_imu_temperature);
    }

    m_data_logger->log("\n");
}

/*******************************************************************************	
 * 
 * Sets the actuator values every period
 * 
 ******************************************************************************/
void ArcadeDriveControl::doPeriodic()
{
	//
	// Read Sensors
	//
    if (m_accelerometer != nullptr)
    {
        m_accelerometer_x = m_accelerometer->GetX();
        m_accelerometer_y = m_accelerometer->GetY();
        m_accelerometer_z = m_accelerometer->GetZ();
    }

    if (m_imu != nullptr)
    {
        m_imu_accel_x      = m_imu->GetAccelX();
        m_imu_accel_y      = m_imu->GetAccelY();
        m_imu_accel_z      = m_imu->GetAccelZ();

        m_imu_mag_x        = m_imu->GetMagX();
        m_imu_mag_y        = m_imu->GetMagY();
        m_imu_mag_z        = m_imu->GetMagZ();

        m_imu_rate_x       = m_imu->GetRateX();
        m_imu_rate_y       = m_imu->GetRateY();
        m_imu_rate_z       = m_imu->GetRateZ();

        m_imu_angle_x      = m_imu->GetAngleX();
        m_imu_angle_y      = m_imu->GetAngleY();
        m_imu_angle_z      = m_imu->GetAngleZ();

        m_imu_roll         = m_imu->GetRoll();
        m_imu_pitch        = m_imu->GetPitch();
        m_imu_yaw          = m_imu->GetYaw();

        m_imu_quat_w       = m_imu->GetQuaternionW();
        m_imu_quat_x       = m_imu->GetQuaternionX();
        m_imu_quat_y       = m_imu->GetQuaternionY();
        m_imu_quat_z       = m_imu->GetQuaternionZ();

        m_imu_bar_press    = m_imu->GetBarometricPressure();
        m_imu_temperature  = m_imu->GetTemperature();
    }

	//
	// Calculate Values
	//
	fl_drive_motor_cmd = RobotUtil::limit(-1.0, 1.0, fwd_power - trn_power);
	fr_drive_motor_cmd = RobotUtil::limit(-1.0, 1.0, fwd_power + trn_power);
	bl_drive_motor_cmd = fl_drive_motor_cmd;
	br_drive_motor_cmd = fr_drive_motor_cmd;

	if (m_low_power_active)
	{
		fl_drive_motor_cmd *= m_low_power_scale;
		fr_drive_motor_cmd *= m_low_power_scale;
	    bl_drive_motor_cmd *= m_low_power_scale;
        br_drive_motor_cmd *= m_low_power_scale;
	}

	if (strafe_state)
	{
		fl_drive_motor_cmd *= -1.0;
		br_drive_motor_cmd *= -1.0;
	}

	// don't drive motors against the brake;
	if (brake_engaged)
	{
		fl_drive_motor_cmd = 0.0;
		fr_drive_motor_cmd = 0.0;
	    bl_drive_motor_cmd = 0.0;
        br_drive_motor_cmd = 0.0;
	}

	//
	// Set Outputs
	//
	if (brake_solenoid != nullptr)
	{
		brake_solenoid->Set(brake_engaged != brake_solenoid_invert);
	}

	if (gear_solenoid != nullptr)
	{
		gear_solenoid->Set(gear_high != gear_solenoid_invert);
	}
	
	if (fl_drive_motor != nullptr)
	{
		fl_drive_motor->Set(fl_drive_motor_cmd);
	}
	
	if (fr_drive_motor != nullptr)
	{
		fr_drive_motor->Set(fr_drive_motor_cmd);
	}
	
	if (bl_drive_motor != nullptr)
	{
		bl_drive_motor->Set(bl_drive_motor_cmd);
	}
	
	if (br_drive_motor != nullptr)
	{
		br_drive_motor->Set(br_drive_motor_cmd);
	}	

	logFileAppend();
}


// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 ******************************************************************************/
MSDriveArcadeDrivePower::MSDriveArcadeDrivePower(std::string type, tinyxml2::XMLElement *xml, void *control)
	: MacroStepSequence(type, xml, control)
{
	parent_control = (ArcadeDriveControl *)control;

	m_forward = xml->FloatAttribute("forward");
	m_turn = xml->FloatAttribute("turn");
	m_strafe = xml->BoolAttribute("strafe");
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSDriveArcadeDrivePower::init(void)
{
	parent_control->setDigital(ArcadeDriveControl::CMD_STRAFE_STATE, m_strafe);
	parent_control->setAnalog(ArcadeDriveControl::CMD_FORWARD, m_forward);
	parent_control->setAnalog(ArcadeDriveControl::CMD_TURN, m_turn);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MSDriveArcadeDrivePower::update(void)
{
	return next_step;
}

