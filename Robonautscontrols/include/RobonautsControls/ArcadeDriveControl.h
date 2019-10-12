/*******************************************************************************
 *
 * File: ArcadeDriveControl.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once


#ifdef _2018
#include "SpeedController.h"
#include "Solenoid.h"
#include "BuiltInAccelerometer.h"
#else
#include "frc/SpeedController.h"
#include "frc/Solenoid.h"
#include "frc/BuiltInAccelerometer.h"
#endif

#include "RobonautsLibrary/RSpeedController.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"
#include "RobonautsLibrary/ADIS16448_IMU.h"
#include "RobonautsLibrary/DataLogger.h"

#include "RobonautsControls/PeriodicControl.h"

/*******************************************************************************
 *
 *  Provides a PeriodicControl implementation for driving the base of a robot
 *  using an Arcade-like driver interface, that is a forward and turn command
 *  are used to make the robot move.
 *  
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="arcade_drive" name="drive">
 *      [<motor name="front_left"   [type="Victor"] [module="1"] [port="1"] [invert="false"] />]
 *      [<motor name="front_right"  [type="Victor"] [module="1"] [port="2"] [invert="false"] />]
 *      [<motor name="back_left"  	[type="Victor"] [module="1"] [port="3"] [invert="false"] />]
 *      [<motor name="back_right" 	[type="Victor"] [module="1"] [port="4"] [invert="false"] />]
 *      [<accelerometer />]
 *      [<imu />]
 *      [<solenoid name="brake" 	[module="1"] 	[port="1"] />]
 *      [<solenoid name="gear"  	[module="1"] 	[port="2"] />]
 *      [<oi name="forward"      	device="pilot" chan="2" 	[scale="-0.8"|invert="false"]/>]
 *      [<oi name="turn"         	device="pilot" chan="3" 	[scale="0.8"|invert="false"]/>]
 *      [<oi name="brake_on"     	device="pilot" chan="1" 	[invert="false"]/>]
 *      [<oi name="brake_off"    	device="pilot" chan="2" 	[invert="false"]/>]
 *      [<oi name="brake_toggle" 	device="pilot" chan="3" 	[invert="false"]/>]
 *      [<oi name="brake_state"  	device="pilot" chan="4" 	[invert="false"]/>]
 *      [<oi name="gear_on"      	device="pilot" chan="5" 	[invert="false"]/>]
 *      [<oi name="gear_off"     	device="pilot" chan="7" 	[invert="false"]/>]
 *      [<oi name="gear_toggle"  	device="pilot" chan="6" 	[invert="false"]/>]
 *      [<oi name="gear_state"   	device="pilot" chan="8" 	[invert="false"]/>]
 *      [<oi name="strafe_on"		device="pilot" chan="9"		[invert="false"]/>]
 *      [<oi name="strafe_off"		device="pilot" chan="10" 	[invert="false"]/>]
 *      [<oi name="strafe_toggle"	device="pilot" chan="11" 	[invert="false"]/>]
 *      [<oi name="strafe_state"	device="pilot" chan="12" 	[invert="false"]/>]
 *  </control>
 * 
 ******************************************************************************/
class ArcadeDriveControl : public PeriodicControl, public OIObserver
{
	public:
		enum
		{
			CMD_FORWARD = 0, 	CMD_TURN,
			CMD_BRAKE_ON, 		CMD_BRAKE_OFF, 	    CMD_BRAKE_TOGGLE, 	    CMD_BRAKE_STATE,
			CMD_GEAR_HIGH, 		CMD_GEAR_LOW, 	    CMD_GEAR_TOGGLE, 	    CMD_GEAR_STATE,
			CMD_LOW_POWER_ON,	CMD_LOW_POWER_OFF, 	CMD_LOW_POWER_TOGGLE, 	CMD_LOW_POWER_STATE,
			CMD_STRAFE_ON,		CMD_STRAFE_OFF,	    CMD_STRAFE_TOGGLE,	    CMD_STRAFE_STATE,
			CMD_ARC,            CMD_POWER_ARC
		};

		ArcadeDriveControl(std::string name, tinyxml2::XMLElement *xml);
		~ArcadeDriveControl(void);

		void setAnalog(int id, float val);
		void setDigital(int id, bool val);
		void setInt(int id, int val);

		void updateConfig();
		void controlInit();

		void disabledInit();
		void autonomousInit();
		void teleopInit();
		void testInit();

		void doPeriodic();
		void publish();

	private:
        void logFileInit(std::string phase);
        void logFileAppend(void);

        DataLogger *m_data_logger;

		RSpeedController *fl_drive_motor;
		RSpeedController *fr_drive_motor;
		RSpeedController *bl_drive_motor;
		RSpeedController *br_drive_motor;

		Solenoid *brake_solenoid;
		Solenoid *gear_solenoid;
		
		float fl_drive_motor_cmd;
		float fr_drive_motor_cmd;
		float bl_drive_motor_cmd;
		float br_drive_motor_cmd;

		bool brake_engaged;
		bool brake_solenoid_invert;

		bool gear_high;
		bool gear_solenoid_invert;
		
		bool strafe_enabled;
		bool strafe_state;
		
		float trn_power;
		float fwd_power;
		float arc_turn_power;
		float arc_fwd_power;
		float arc_turn_scale;

		float m_low_power_scale;
		bool m_low_power_active;

		int arc_input;
		bool arc_power;

        Accelerometer *m_accelerometer;
        double m_accelerometer_x;
        double m_accelerometer_y;
        double m_accelerometer_z;

        ADIS16448_IMU *m_imu;
        double m_imu_accel_x;
        double m_imu_accel_y;
        double m_imu_accel_z;

        double m_imu_mag_x;
        double m_imu_mag_y;
        double m_imu_mag_z;

        double m_imu_rate_x;
        double m_imu_rate_y;
        double m_imu_rate_z;

        double m_imu_angle_x;
        double m_imu_angle_y;
        double m_imu_angle_z;

        double m_imu_roll;
        double m_imu_pitch;
        double m_imu_yaw;

        double m_imu_quat_w;
        double m_imu_quat_x;
        double m_imu_quat_y;
        double m_imu_quat_z;

        double m_imu_bar_press;
        double m_imu_temperature;
};


// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 * This Macro Step sets the forward power, turn power, and strafe state of the
 * Drive Arcade PacBotControl.
 *
 * WARNING: This just sets the power, it does not turn them off unless the
 *          provided values for forward, turn, and strafe turn the drive off.
 *
 *  Example XML:
 *
 *	<step name="drive_1" control="drive" type="DrivePower"
 *			forward="0.5" turn="0.1" strafe="false">
 *  	<connect type="next" step="drive_wait"/>>
 *  </step>
 *
 ******************************************************************************/
class MSDriveArcadeDrivePower : public MacroStepSequence
{
	public:
		MSDriveArcadeDrivePower(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		ArcadeDriveControl *parent_control;

		float m_forward;
		float m_turn;
		bool  m_strafe;
};

