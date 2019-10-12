/*******************************************************************************
 *
 * File: RDigitalInput.h -- Digital Input for Simple Switches
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

//#include "WPILib.h"
#ifdef _2018
#include "SpeedController.h"
#include "PowerDistributionPanel.h"
#else
#include "frc/SpeedController.h"
#include "frc/PowerDistributionPanel.h"

#endif

#include "ctre/Phoenix.h"

#define kTimeout 0

using namespace frc;

/*******************************************************************************
 *
 * This class provides the definition of an interface for speed controllers,
 * providing a common interface for both PWM speed controller and CAN Talon
 * speed controllers.
 * 
 ******************************************************************************/
class RSpeedController
{
	public:
		enum FeedbackDeviceType
		{
			EXTERNAL_ENCODER=0,
			INTERNAL_ENCODER,
			EXTERNAL_POTENTIOMETER,
			INTERNAL_POTENTIOMETER,
		};

		enum ControlModeType
		{
			DUTY_CYCLE=0,
			POSITION,
			VELOCITY,
			TRAPEZOID,
//			CURRENT,
			FOLLOWER,
			UNKNOWN_TYPE
		};

        virtual ~RSpeedController(void);

        virtual bool SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a=0, int port_b=0) = 0;
		virtual bool InvertMotor(bool invert) = 0;
		virtual void InvertSensor(bool invert, bool is_drive=false) = 0;
		virtual void SensorOutputPerCount(double conversion, double offset=0) = 0;

        virtual void SetBrakeMode(bool brake_mode) {}

		virtual bool SetControlMode(ControlModeType type, int deviceID=-1) = 0;
        virtual ControlModeType GetControlMode() = 0;

		virtual void Set(double val) = 0;  // can be duty cycle, position or velocity or device to follow
		virtual double Get() = 0;  // can be duty cycle, position or velocity or device to follow

		virtual void SetPosition(double pos)= 0;
		virtual double GetPosition() = 0;
		virtual int32_t GetRawPosition(void) = 0;

		virtual double GetActiveTrajectoryPosition() = 0;
		virtual double GetActiveTrajectoryVelocity() = 0;
		virtual void SetClosedLoopOutputLimits(float fwd_nom, float rev_nom, float fwd_peak, float rev_peak) {}
		virtual double GetSpeed() = 0;
		virtual int32_t GetRawSpeed(void) = 0;

		virtual void SetForwardLimitSwitch (bool normally_open, bool enabled = true, bool zero_position = false) = 0;
		virtual void SetReverseLimitSwitch  (bool normally_open, bool enabled = true, bool zero_position = false) = 0;
		virtual int GetDeviceID() = 0;

		virtual void SetCurrentLimit(uint32_t amps, uint32_t peak=0, uint32_t duration=0 ) = 0;
		virtual void SetCurrentLimitEnabled(bool enabled) = 0;

		virtual double GetBusVoltage() = 0;
		virtual double GetOutputVoltage() = 0;
		virtual double GetMotorOutputPercent() = 0;
		virtual double GetOutputCurrent() = 0;

		virtual void SetP(double p) = 0;
		virtual void SetI(double i) = 0;
		virtual void SetD(double d) = 0;
		virtual void SetF(double f) = 0;
        virtual void SetIZone(double i_zone) = 0;

		virtual void SetCruiseVelocity(double vel) = 0;
		virtual void SetAcceleration(double accel) = 0;

		virtual double GetP() = 0;
		virtual double GetI() = 0;
		virtual double GetD() = 0;
		virtual double GetF() = 0;

	protected:
        RSpeedController(void);

        ControlModeType m_mode;
		int m_control_period_ms;
};

/*******************************************************************************
 *
 * This subclass is for CAN controllers
 *
 ******************************************************************************/
class RSpeedControllerCan : public RSpeedController
{
    public:
        RSpeedControllerCan(int port_device, int controlPeriodMs=10);
        ~RSpeedControllerCan();

        bool SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a=0, int port_b=0);
        bool InvertMotor(bool invert);
        void SetBrakeMode(bool brake_mode);

        bool SetControlMode(ControlModeType type, int deviceID=-1);
        ControlModeType GetControlMode();

        void InvertSensor(bool invert, bool is_drv=false);
        void SensorOutputPerCount(double conversion, double offset=0);

        void SetClosedLoopOutputLimits(float min_nom, float max_nom, float min_peak, float max_peak);

        void Set(double val);  // can be duty cycle, position or velocity or device to follow
        double Get();  // can be duty cycle, position or velocity or device to follow

        void SetPosition(double pos);
        double GetPosition();
        int32_t GetRawPosition(void);

        double GetSpeed();
		int32_t GetRawSpeed(void);

        int GetDeviceID();

        void SetCurrentLimit(uint32_t amps, uint32_t peak=0, uint32_t duration=0 );
        void SetCurrentLimitEnabled(bool enabled);
        void SetForwardLimitSwitch (bool normally_open, bool enabled = true, bool zero_position = false);
        void SetReverseLimitSwitch  (bool normally_open, bool enabled = true, bool zero_position = false);

        double GetBusVoltage();
        double GetOutputVoltage();
		double GetMotorOutputPercent();
        double GetOutputCurrent();
		double GetActiveTrajectoryPosition();
		double GetActiveTrajectoryVelocity();

        void SetP(double p);
        void SetI(double i);
        void SetD(double d);
        void SetF(double f);
        void SetIZone(double i_zone);
		void SetCruiseVelocity(double vel);
		void SetAcceleration(double accel);

        double GetP();
        double GetI();
        double GetD();
        double GetF();

    private:
        TalonSRX m_controller;
        double m_sensor_invert;
        double m_output_per_count;
        double m_output_offset;
};

/*******************************************************************************
 *
 * This subclass is for CAN controllers
 *
 ******************************************************************************/
class RSpeedControllerVictorSpxCan : public RSpeedController
{
    public:
        RSpeedControllerVictorSpxCan(int port_device, int controlPeriodMs=10);
        ~RSpeedControllerVictorSpxCan();

        bool SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a=0, int port_b=0);
        bool InvertMotor(bool invert);
        void SetBrakeMode(bool brake_mode);

        bool SetControlMode(ControlModeType type, int deviceID=-1);
        ControlModeType GetControlMode();

        void InvertSensor(bool invert, bool is_drv=false);
        void SensorOutputPerCount(double conversion, double offset=0);

        void SetClosedLoopOutputLimits(float min_nom, float max_nom, float min_peak, float max_peak);

        void Set(double val);  // can be duty cycle, position or velocity or device to follow
        double Get();  // can be duty cycle, position or velocity or device to follow

        void SetPosition(double pos);
        double GetPosition();
        int32_t GetRawPosition(void);

        double GetSpeed();
		int32_t GetRawSpeed(void);

        int GetDeviceID();

        void SetCurrentLimit(uint32_t amps, uint32_t peak=0, uint32_t duration=0 );
        void SetCurrentLimitEnabled(bool enabled);
        void SetForwardLimitSwitch (bool normally_open, bool enabled = true, bool zero_position = false);
        void SetReverseLimitSwitch  (bool normally_open, bool enabled = true, bool zero_position = false);

        double GetBusVoltage();
        double GetOutputVoltage();
		double GetMotorOutputPercent();
        double GetOutputCurrent();
		double GetActiveTrajectoryPosition();
		double GetActiveTrajectoryVelocity();

        void SetP(double p);
        void SetI(double i);
        void SetD(double d);
        void SetF(double f);
        void SetIZone(double i_zone);
		void SetCruiseVelocity(double vel);
		void SetAcceleration(double accel);

        double GetP();
        double GetI();
        double GetD();
        double GetF();

    private:
        VictorSPX m_controller;
        double m_sensor_invert;
        double m_output_per_count;
        double m_output_offset;
};

/*******************************************************************************
 *
 * This subclass is for PWM controllers
 *
 ******************************************************************************/
class RSpeedControllerPwm : public RSpeedController
{
    public:
        RSpeedControllerPwm(SpeedController *sc, int controlPeriodMs);
        ~RSpeedControllerPwm();

        bool SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a=0, int port_b=0);
        bool InvertMotor(bool invert);

        bool SetControlMode(ControlModeType type, int deviceID=-1);
        ControlModeType GetControlMode();

        void SetFuse(int fuse);

        void InvertSensor(bool invert, bool is_drv=false);
        void SensorOutputPerCount(double conversion, double offset);

        void Set(double val);  // can be duty cycle, position or velocity or device to follow
        double Get();  // can be duty cycle, position or velocity or device to follow
        void SetPosition(double pos);

        double GetPosition();
        int32_t GetRawPosition(void);

        double GetSpeed();
		int32_t GetRawSpeed(void);
        int GetDeviceID();

        void SetCurrentLimit(uint32_t amps, uint32_t peak=0, uint32_t duration=0 );
        void SetCurrentLimitEnabled(bool enabled);
        void SetForwardLimitSwitch (bool normally_open, bool enabled = true, bool zero_position = false);
        void SetReverseLimitSwitch  (bool normally_open, bool enabled = true, bool zero_position = false);

        double GetBusVoltage();
        double GetOutputVoltage();
		double GetMotorOutputPercent();
        double GetOutputCurrent();
		double GetActiveTrajectoryPosition() { return 0; };
		double GetActiveTrajectoryVelocity() { return 0; };

        void SetP(double p);
        void SetI(double i);
        void SetD(double d);
        void SetF(double f);
        void SetIZone(double i_zone);
		void SetCruiseVelocity(double vel);
		void SetAcceleration(double accel);

        double GetP();
        double GetI();
        double GetD();
        double GetF();

    private:
        SpeedController *m_controller;
        PowerDistributionPanel m_power_panel;
        int m_fuse;
        double m_sensor_invert;
        double m_output_per_count;
        double m_output_offset;

        uint32_t m_current_limit;
        bool m_current_limit_enabled;
};

