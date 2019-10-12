/*******************************************************************************
 *
 * File: RAbsPosSensor.cpp
 * 
 * This file contains the definition of a class for interacting with an
 * Absolute Position Sensor.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RSpeedController.h"
#include "gsu/Advisory.h"

/******************************************************************************
 * 
 * Create an interface of RSpeedController, geto an APS plugged into the specified channel.
 * 
 * @param	channel	the channel or port that the pot is in
 * 
 ******************************************************************************/
RSpeedController::RSpeedController(void)
{
    m_mode = ControlModeType::DUTY_CYCLE;
	m_control_period_ms=10;
}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedController::~RSpeedController(void)
{
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

/******************************************************************************
 *
 * Create an interface of RSpeedControllerCan, get an APS plugged into the specified channel.
 *
 * @param   channel the channel or port that the pot is in
 *
 ******************************************************************************/
RSpeedControllerCan::RSpeedControllerCan(int port_device, int controlPeriodMs)
    : RSpeedController()
, m_controller(port_device)  // TODO: figure out what to do with controlPeriodMs
//, m_controller(port_device, controlPeriodMs)
{
    m_sensor_invert = 1.0;  // 1.0 if not inverted, -1.0 if inverted
    m_output_per_count = 1.0;
    m_output_offset = 0.0;
    m_control_period_ms = controlPeriodMs;
    m_controller.SetStatusFramePeriod(StatusFrameEnhanced::Status_1_General, controlPeriodMs, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedControllerCan::~RSpeedControllerCan()
{

}

/******************************************************************************
 *
 *
 * If sensor_type is INTERNAL_POTENTIOMETER:
 *     port_a==0 means no sensor wrap, port_a!=0 means sensor wrap
 *
 ******************************************************************************/
bool RSpeedControllerCan::SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a, int port_b)
{
    bool ret_val = false;

    switch(sensor_type)
    {
        case INTERNAL_ENCODER:
//            m_controller.SetFeedbackDevice(CANTalon::QuadEncoder);
            m_controller.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, kTimeout);
            ret_val = true;
            break;

        case INTERNAL_POTENTIOMETER:
            m_controller.ConfigSelectedFeedbackSensor(FeedbackDevice::Analog, 0, kTimeout);
            m_controller.ConfigSetParameter(ParamEnum::eFeedbackNotContinuous, (port_a==0), 0x00, 0x00, 0x00);
            ret_val = true;
            break;

        case EXTERNAL_ENCODER:
            // create encoder using A/B, set duty cycle mode
            break;

        case EXTERNAL_POTENTIOMETER:
            // create rpot
            break;

        default:
            ret_val = false;
    }

    return(ret_val);
}

/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerCan::InvertMotor(bool invert)
{
	m_controller.SetInverted(invert);
    return(true);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SetBrakeMode(bool brake_mode)
{
    if (brake_mode)
    {
        m_controller.SetNeutralMode(NeutralMode::Brake);
    }
    else
    {
        m_controller.SetNeutralMode(NeutralMode::Coast);
    }
}

/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerCan::SetControlMode(ControlModeType type, int device_id)
{
    bool ret_val = true;

    m_mode = type;
    switch(type)
    {
        case DUTY_CYCLE:
        case POSITION:
        case VELOCITY:
        case TRAPEZOID:
        	m_controller.SetSelectedSensorPosition(m_controller.GetSelectedSensorPosition(0),0,kTimeout);
        	break;

        case FOLLOWER:
            m_controller.Set(ControlMode::Follower, device_id);
            break;

        default:
        	m_mode = DUTY_CYCLE;
            ret_val = false;
            break;
    }

    return(ret_val);
}

// Change: was +/-12V, now +/-1.0 (percent)
void RSpeedControllerCan::SetClosedLoopOutputLimits(float fwd_nom, float rev_nom, float fwd_peak, float rev_peak)
{
	m_controller.ConfigPeakOutputForward(fwd_peak, kTimeout);
	m_controller.ConfigPeakOutputReverse(rev_peak, kTimeout);
	m_controller.ConfigNominalOutputForward(fwd_nom, kTimeout);
	m_controller.ConfigNominalOutputReverse(rev_nom, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::InvertSensor(bool invert, bool is_drv)
{
	if(false == is_drv)
	{
        m_sensor_invert = 1.0;
        m_controller.SetSensorPhase(invert);
//        m_controller.SetSensorDirection(invert);
	}
	else
	{
		if(invert == true)
		{
			m_sensor_invert = -1.0;
		}
		else
		{
			m_sensor_invert = 1.0;
		}
	}
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SensorOutputPerCount(double conversion, double offset)
{
    m_output_per_count = conversion;
    m_output_offset = offset;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::Set(double val)  // can be duty cycle, position or velocity or device to follow
{
    if(m_mode == ControlModeType::VELOCITY)
    {
    	//Advisory::pinfo(" Set(velocity (%f , %f) (opc %f):" ,
    	//		val, val/m_output_per_count / 10.0, m_output_per_count);
        m_controller.Set(ControlMode::Velocity, val/m_output_per_count / 10.0);  //  TODO fix when units better understood
    }
    else if (m_mode == ControlModeType::POSITION)
    {
 //   	Advisory::pinfo("%s:%d Set(position (%f - %f)/%f = %f", __FILE__, __LINE__,
 //   			val, m_output_offset, m_output_per_count, (val-m_output_offset)/m_output_per_count);
    	m_controller.Set(ControlMode::Position, (val-m_output_offset)/m_output_per_count);
    }
    else if (m_mode == ControlModeType::TRAPEZOID)
    {
//    	Advisory::pinfo("%s:%d Set(trapezoid (%f - %f)/%f = %f", __FILE__, __LINE__,
//    			val, m_output_offset, m_output_per_count, (val-m_output_offset)/m_output_per_count);
    	m_controller.Set(ControlMode::MotionMagic, (val-m_output_offset)/m_output_per_count);
    }
    else
    {
        m_controller.Set(ControlMode::PercentOutput, val);  //  TODO fix when units better understood
   }
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::Get()  // can be duty cycle, position or velocity or device to follow
{
//    return  m_controller.Get();
    return  0.0;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SetPosition(double pos)
{
	m_controller.SetSelectedSensorPosition(((pos-m_output_offset)/m_output_per_count), 0, kTimeout);
}

int32_t RSpeedControllerCan::GetRawPosition(void)
{
	return m_controller.GetSelectedSensorPosition(0);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetPosition()
{
	return ( ((m_controller.GetSelectedSensorPosition(0) * m_output_per_count) + m_output_offset) );
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t RSpeedControllerCan::GetRawSpeed()
{
	return m_controller.GetSelectedSensorVelocity(0);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetSpeed()
{
//    double raw = m_controller.GetSpeed();
    double raw = m_controller.GetSelectedSensorVelocity(0);

//    int err = m_controller.GetClosedLoopError();
//    Advisory::pinfo("RSpeedControllerCan::GetSpeed() %f * %f * %f / 0.1 = %f, err=%d", raw, m_sensor_invert,
//        m_output_per_count, (raw * m_sensor_invert * m_output_per_count / 0.1), err);

    return (raw * m_sensor_invert * m_output_per_count / 0.1);  // todo no hard code
}

/******************************************************************************
 *
 ******************************************************************************/
int RSpeedControllerCan::GetDeviceID()
{
    return m_controller.GetDeviceID();
}

/******************************************************************************
 *
 ******************************************************************************/
void  RSpeedControllerCan::SetCurrentLimit(uint32_t amps, uint32_t peak, uint32_t duration )
{
    m_controller.ConfigContinuousCurrentLimit(amps, kTimeout);
    m_controller.ConfigPeakCurrentLimit(peak, kTimeout);
    m_controller.ConfigPeakCurrentDuration(amps, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void  RSpeedControllerCan::SetCurrentLimitEnabled(bool enabled)
{
    m_controller.EnableCurrentLimit(enabled);
}

/******************************************************************************
 *
 * Configuring the Can Limit Switch
 *
 * @param normally_open 	true if the switch is wired to be normally open,
 *                          false if it is normally closed
 *
 * @param enabled			if true (the default) it will the talon will be
 * 							configured to have a limit switch wired to the
 * 							speed controller, if false it will turn off
 * 							the limit switch
 * 							NOTE: this could be updated to also allow for
 * 							the Talon to use external limit switches.
 *
 ******************************************************************************/
void RSpeedControllerCan::SetForwardLimitSwitch (bool normally_open, bool enabled, bool zero_position)
{
	Advisory::pinfo("RSpeedControllerCan::SetForwardLimitSwitch(%d,%d,%d)", normally_open,enabled);
	if (enabled)
	{
		m_controller.ConfigForwardLimitSwitchSource(
				LimitSwitchSource_FeedbackConnector,
				normally_open?LimitSwitchNormal_NormallyOpen:LimitSwitchNormal_NormallyClosed,
				kTimeout);

        m_controller.ConfigClearPositionOnLimitF(zero_position);
	}
	else
	{
		m_controller.ConfigForwardLimitSwitchSource(
				LimitSwitchSource_Deactivated,
				LimitSwitchNormal_Disabled,
				kTimeout);
	}
}

/******************************************************************************
 *
 * Configuring the Can Limit Switch
 *
 * @param normally_open 	true if the switch is wired to be normally open,
 *                          false if it is normally closed
 *
 * @param enabled			if true (the default) it will the talon will be
 * 							configured to have a limit switch wired to the
 * 							speed controller, if false it will turn off
 * 							the limit switch
 * 							NOTE: this could be updated to also allow for
 * 							the Talon to use external limit switches.
 *
 ******************************************************************************/
void RSpeedControllerCan::SetReverseLimitSwitch  (bool normally_open, bool enabled, bool zero_position)
{
	if (enabled)
	{
		m_controller.ConfigReverseLimitSwitchSource(
				LimitSwitchSource_FeedbackConnector,
				normally_open?LimitSwitchNormal_NormallyOpen:LimitSwitchNormal_NormallyClosed,
				kTimeout);

        m_controller.ConfigClearPositionOnLimitR(zero_position);	
    }
	else
	{
		m_controller.ConfigReverseLimitSwitchSource(
				LimitSwitchSource_Deactivated,
				LimitSwitchNormal_Disabled,
				kTimeout);
	}

}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetBusVoltage()
{
    return m_controller.GetBusVoltage();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetOutputVoltage()
{
    return m_controller.GetMotorOutputVoltage();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetMotorOutputPercent()
{
    return m_controller.GetMotorOutputPercent();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetOutputCurrent()
{
    return m_controller.GetOutputCurrent();
}

double RSpeedControllerCan::GetActiveTrajectoryPosition()
{
	return m_controller.GetActiveTrajectoryPosition();
}

double RSpeedControllerCan::GetActiveTrajectoryVelocity()
{
	return m_controller.GetActiveTrajectoryVelocity();

}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedController::ControlModeType RSpeedControllerCan::GetControlMode()
{
	return m_mode;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SetP(double p)
{
//     m_controller.SetP(p);
     m_controller.Config_kP(0, p, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SetI(double i)
{
//    m_controller.SetI(i);
    m_controller.Config_kI(0, i, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SetD(double d)
{
//    m_controller.SetD(d);
    m_controller.Config_kD(0, d, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerCan::SetF(double f)
{
//    m_controller.SetF(f);
    m_controller.Config_kF(0, f, kTimeout);
}

void RSpeedControllerCan::SetIZone(double i_zone)
{
	m_controller.Config_IntegralZone(0, (int)i_zone, kTimeout);
}

/******************************************************************************
 *
 * @param vel units/sec, where units are user units based on the value(s)
 * specified in the call to SensorOutputPerCount()
 *
 ******************************************************************************/
void RSpeedControllerCan::SetCruiseVelocity(double vel)
{
	m_controller.ConfigMotionCruiseVelocity((int)(vel/m_output_per_count / 10.0), kTimeout);
}

/******************************************************************************
 *
 * @param accel (units/sec)/sec, where units are user units based on the value(s)
 * specified in the call to SensorOutputPerCount()
 *
 ******************************************************************************/
void RSpeedControllerCan::SetAcceleration(double accel)
{
	m_controller.ConfigMotionAcceleration((int)(accel/m_output_per_count / 10.0), kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
// Look at ConfigGetParameter()
double RSpeedControllerCan::GetP()
{
    return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_P, 0, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetI()
{
    return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_I, 0, kTimeout);
//    return m_controller.GetI();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetD()
{
	return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_D, 0, kTimeout);
//    return  m_controller.GetD();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerCan::GetF()
{
	return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_F, 0, kTimeout);
//    return m_controller.GetF();
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

/******************************************************************************
 *
 * Create an interface of RSpeedControllerCan, get an APS plugged into the specified channel.
 *
 * @param   channel the channel or port that the pot is in
 *
 ******************************************************************************/
RSpeedControllerVictorSpxCan::RSpeedControllerVictorSpxCan(int port_device, int controlPeriodMs)
    : RSpeedController()
, m_controller(port_device)  // TODO: figure out what to do with controlPeriodMs
//, m_controller(port_device, controlPeriodMs)
{
    m_sensor_invert = 1.0;  // 1.0 if not inverted, -1.0 if inverted
    m_output_per_count = 1.0;
    m_output_offset = 0.0;
    m_control_period_ms = controlPeriodMs;
    m_controller.SetStatusFramePeriod(StatusFrameEnhanced::Status_1_General, controlPeriodMs, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedControllerVictorSpxCan::~RSpeedControllerVictorSpxCan()
{

}

/******************************************************************************
 *
 *
 * If sensor_type is INTERNAL_POTENTIOMETER:
 *     port_a==0 means no sensor wrap, port_a!=0 means sensor wrap
 *
 ******************************************************************************/
bool RSpeedControllerVictorSpxCan::SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a, int port_b)
{
    bool ret_val = false;

    switch(sensor_type)
    {
        case INTERNAL_ENCODER:
//            m_controller.SetFeedbackDevice(CANTalon::QuadEncoder);
            m_controller.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, kTimeout);
            ret_val = true;
            break;

        case INTERNAL_POTENTIOMETER:
            m_controller.ConfigSelectedFeedbackSensor(FeedbackDevice::Analog, 0, kTimeout);
            m_controller.ConfigSetParameter(ParamEnum::eFeedbackNotContinuous, (port_a==0), 0x00, 0x00, 0x00);
            ret_val = true;
            break;

        case EXTERNAL_ENCODER:
            // create encoder using A/B, set duty cycle mode
            break;

        case EXTERNAL_POTENTIOMETER:
            // create rpot
            break;

        default:
            ret_val = false;
    }

    return(ret_val);
}

/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerVictorSpxCan::InvertMotor(bool invert)
{
	m_controller.SetInverted(invert);
    return(true);
}
/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetBrakeMode(bool brake_mode)
{
    if (brake_mode)
    {
        m_controller.SetNeutralMode(NeutralMode::Brake);
    }
    else
    {
        m_controller.SetNeutralMode(NeutralMode::Coast);
    }
}


/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerVictorSpxCan::SetControlMode(ControlModeType type, int device_id)
{
    bool ret_val = true;

    m_mode = type;
    switch(type)
    {
        case DUTY_CYCLE:
        case POSITION:
        case VELOCITY:
        case TRAPEZOID:
        	m_controller.SetSelectedSensorPosition(m_controller.GetSelectedSensorPosition(0),0,kTimeout);
        	break;

        case FOLLOWER:
            m_controller.Set(ControlMode::Follower, device_id);
            break;

        default:
        	m_mode = DUTY_CYCLE;
            ret_val = false;
            break;
    }

    return(ret_val);
}

// Change: was +/-12V, now +/-1.0 (percent)
void RSpeedControllerVictorSpxCan::SetClosedLoopOutputLimits(float fwd_nom, float rev_nom, float fwd_peak, float rev_peak)
{
	m_controller.ConfigPeakOutputForward(fwd_peak, kTimeout);
	m_controller.ConfigPeakOutputReverse(rev_peak, kTimeout);
	m_controller.ConfigNominalOutputForward(fwd_nom, kTimeout);
	m_controller.ConfigNominalOutputReverse(rev_nom, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::InvertSensor(bool invert, bool is_drv)
{
	if(false == is_drv)
	{
        m_sensor_invert = 1.0;
        m_controller.SetSensorPhase(invert);
//        m_controller.SetSensorDirection(invert);
	}
	else
	{
		if(invert == true)
		{
			m_sensor_invert = -1.0;
		}
		else
		{
			m_sensor_invert = 1.0;
		}
	}
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SensorOutputPerCount(double conversion, double offset)
{
    m_output_per_count = conversion;
    m_output_offset = offset;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::Set(double val)  // can be duty cycle, position or velocity or device to follow
{
    if(m_mode == ControlModeType::VELOCITY)
    {
    	//Advisory::pinfo(" Set(velocity (%f , %f) (opc %f):" ,
    	//		val, val/m_output_per_count / 10.0, m_output_per_count);
        m_controller.Set(ControlMode::Velocity, val/m_output_per_count / 10.0);  //  TODO fix when units better understood
    }
    else if (m_mode == ControlModeType::POSITION)
    {
 //   	Advisory::pinfo("%s:%d Set(position (%f - %f)/%f = %f", __FILE__, __LINE__,
 //   			val, m_output_offset, m_output_per_count, (val-m_output_offset)/m_output_per_count);
    	m_controller.Set(ControlMode::Position, (val-m_output_offset)/m_output_per_count);
    }
    else if (m_mode == ControlModeType::TRAPEZOID)
    {
//    	Advisory::pinfo("%s:%d Set(trapezoid (%f - %f)/%f = %f", __FILE__, __LINE__,
//    			val, m_output_offset, m_output_per_count, (val-m_output_offset)/m_output_per_count);
    	m_controller.Set(ControlMode::MotionMagic, (val-m_output_offset)/m_output_per_count);
    }
    else
    {
        m_controller.Set(ControlMode::PercentOutput, val);  //  TODO fix when units better understood
   }
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::Get()  // can be duty cycle, position or velocity or device to follow
{
//    return  m_controller.Get();
    return  0.0;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetPosition(double pos)
{
	m_controller.SetSelectedSensorPosition(((pos-m_output_offset)/m_output_per_count), 0, kTimeout);
}

int32_t RSpeedControllerVictorSpxCan::GetRawPosition(void)
{
	return m_controller.GetSelectedSensorPosition(0);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetPosition()
{
	return ( ((m_controller.GetSelectedSensorPosition(0) * m_output_per_count) + m_output_offset) );
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t RSpeedControllerVictorSpxCan::GetRawSpeed()
{
	return m_controller.GetSelectedSensorVelocity(0);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetSpeed()
{
//    double raw = m_controller.GetSpeed();
    double raw = m_controller.GetSelectedSensorVelocity(0);

//    int err = m_controller.GetClosedLoopError();
//    Advisory::pinfo("RSpeedControllerCan::GetSpeed() %f * %f * %f / 0.1 = %f, err=%d", raw, m_sensor_invert,
//        m_output_per_count, (raw * m_sensor_invert * m_output_per_count / 0.1), err);

    return (raw * m_sensor_invert * m_output_per_count / 0.1);  // todo no hard code
}

/******************************************************************************
 *
 ******************************************************************************/
int RSpeedControllerVictorSpxCan::GetDeviceID()
{
    return m_controller.GetDeviceID();
}

/******************************************************************************
 *
 ******************************************************************************/
void  RSpeedControllerVictorSpxCan::SetCurrentLimit(uint32_t amps, uint32_t peak, uint32_t duration )
{
//    m_controller.ConfigContinuousCurrentLimit(amps, kTimeout);
//    m_controller.ConfigPeakCurrentLimit(peak, kTimeout);
//    m_controller.ConfigPeakCurrentDuration(amps, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void  RSpeedControllerVictorSpxCan::SetCurrentLimitEnabled(bool enabled)
{
//    m_controller.EnableCurrentLimit(enabled);
}

/******************************************************************************
 *
 * Configuring the Can Limit Switch
 *
 * @param normally_open 	true if the switch is wired to be normally open,
 *                          false if it is normally closed
 *
 * @param enabled			if true (the default) it will the talon will be
 * 							configured to have a limit switch wired to the
 * 							speed controller, if false it will turn off
 * 							the limit switch
 * 							NOTE: this could be updated to also allow for
 * 							the Talon to use external limit switches.
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetForwardLimitSwitch (bool normally_open, bool enabled, bool zero_position)
{
	Advisory::pinfo("RSpeedControllerCan::SetForwardLimitSwitch(%d,%d)", normally_open,enabled);
	if (enabled)
	{
		m_controller.ConfigForwardLimitSwitchSource(
				LimitSwitchSource_FeedbackConnector,
				normally_open?LimitSwitchNormal_NormallyOpen:LimitSwitchNormal_NormallyClosed,
				kTimeout);
                
        m_controller.ConfigClearPositionOnLimitF(zero_position);
	}
	else
	{
		m_controller.ConfigForwardLimitSwitchSource(
				LimitSwitchSource_Deactivated,
				LimitSwitchNormal_Disabled,
				kTimeout);
	}
}

/******************************************************************************
 *
 * Configuring the Can Limit Switch
 *
 * @param normally_open 	true if the switch is wired to be normally open,
 *                          false if it is normally closed
 *
 * @param enabled			if true (the default) it will the talon will be
 * 							configured to have a limit switch wired to the
 * 							speed controller, if false it will turn off
 * 							the limit switch
 * 							NOTE: this could be updated to also allow for
 * 							the Talon to use external limit switches.
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetReverseLimitSwitch  (bool normally_open, bool enabled, bool zero_position)
{
	if (enabled)
	{
		m_controller.ConfigReverseLimitSwitchSource(
				LimitSwitchSource_FeedbackConnector,
				normally_open?LimitSwitchNormal_NormallyOpen:LimitSwitchNormal_NormallyClosed,
				kTimeout);
                
        m_controller.ConfigClearPositionOnLimitR(zero_position);
	}
	else
	{
		m_controller.ConfigReverseLimitSwitchSource(
				LimitSwitchSource_Deactivated,
				LimitSwitchNormal_Disabled,
				kTimeout);
	}

}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetBusVoltage()
{
    return m_controller.GetBusVoltage();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetOutputVoltage()
{
    return m_controller.GetMotorOutputVoltage();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetMotorOutputPercent()
{
    return m_controller.GetMotorOutputPercent();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetOutputCurrent()
{
    return 0.0;
//    return m_controller.GetOutputCurrent();
}

double RSpeedControllerVictorSpxCan::GetActiveTrajectoryPosition()
{
	return m_controller.GetActiveTrajectoryPosition();
}

double RSpeedControllerVictorSpxCan::GetActiveTrajectoryVelocity()
{
	return m_controller.GetActiveTrajectoryVelocity();

}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedController::ControlModeType RSpeedControllerVictorSpxCan::GetControlMode()
{
	return m_mode;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetP(double p)
{
//     m_controller.SetP(p);
     m_controller.Config_kP(0, p, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetI(double i)
{
//    m_controller.SetI(i);
    m_controller.Config_kI(0, i, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetD(double d)
{
//    m_controller.SetD(d);
    m_controller.Config_kD(0, d, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetF(double f)
{
//    m_controller.SetF(f);
    m_controller.Config_kF(0, f, kTimeout);
}

void RSpeedControllerVictorSpxCan::SetIZone(double i_zone)
{
	m_controller.Config_IntegralZone(0, (int)i_zone, kTimeout);
}

/******************************************************************************
 *
 * @param vel units/sec, where units are user units based on the value(s)
 * specified in the call to SensorOutputPerCount()
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetCruiseVelocity(double vel)
{
	m_controller.ConfigMotionCruiseVelocity((int)(vel/m_output_per_count / 10.0), kTimeout);
}

/******************************************************************************
 *
 * @param accel (units/sec)/sec, where units are user units based on the value(s)
 * specified in the call to SensorOutputPerCount()
 *
 ******************************************************************************/
void RSpeedControllerVictorSpxCan::SetAcceleration(double accel)
{
	m_controller.ConfigMotionAcceleration((int)(accel/m_output_per_count / 10.0), kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
// Look at ConfigGetParameter()
double RSpeedControllerVictorSpxCan::GetP()
{
    return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_P, 0, kTimeout);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetI()
{
    return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_I, 0, kTimeout);
//    return m_controller.GetI();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetD()
{
	return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_D, 0, kTimeout);
//    return  m_controller.GetD();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerVictorSpxCan::GetF()
{
	return  m_controller.ConfigGetParameter(ParamEnum::eProfileParamSlot_F, 0, kTimeout);
//    return m_controller.GetF();
}


//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================


/******************************************************************************
 *
 * Create an interface of RSpeedControllerCan, get an APS plugged into the specified channel.
 *
 * @param   channel the channel or port that the pot is in
 *
 ******************************************************************************/
RSpeedControllerPwm::RSpeedControllerPwm(SpeedController *sc, int controlPeriodMs)
    : RSpeedController()
    , m_controller(sc)
{
    m_sensor_invert = 1.0;  // 1.0 if not inverted, -1.0 if inverted
    m_output_per_count = 1.0;
    m_fuse = 0;

    m_output_offset = 0.0;
    m_current_limit = 999.999;
    m_current_limit_enabled = false;
}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedControllerPwm::~RSpeedControllerPwm()
{
    if (m_controller != nullptr)
    {
        delete m_controller;
        m_controller = nullptr;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerPwm::SetFeedbackDevice(FeedbackDeviceType sensor_type, int port_a, int port_b)
{
    bool ret_val = true;


    switch(sensor_type)
    {
        case INTERNAL_ENCODER:
            ret_val = false;
            break;
        case INTERNAL_POTENTIOMETER:
            ret_val = false;
            break;
        case EXTERNAL_ENCODER:
            // create encoder using A/B, set duty cycle mode
            break;
        case EXTERNAL_POTENTIOMETER:
            // creat rpot
            break;
        default:
                ret_val = false;
                break;
    }


    return(ret_val);
}

/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerPwm::InvertMotor(bool invert)
{
        m_controller->SetInverted(invert);
    return(true);
}

/******************************************************************************
 *
 ******************************************************************************/
bool RSpeedControllerPwm::SetControlMode(ControlModeType type, int deviceID)
{
    return(false);
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::InvertSensor(bool invert, bool is_drv)
{
    if(invert == true)
    {
        m_sensor_invert = -1.0;
    }
    else
    {
        m_sensor_invert = 1.0;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SensorOutputPerCount(double conversion, double offset)
{
    m_output_per_count = conversion;
    m_output_offset = offset;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::Set(double val)  // can be duty cycle, position or velocity or device to follow
{
        m_controller->Set(val);
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::Get()  // can be duty cycle, position or velocity or device to follow
{
    return  m_controller->Get();
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetPosition(double pos)
{
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetFuse(int fuse)
{
	m_fuse = fuse;
}


/******************************************************************************
 *
 ******************************************************************************/
int32_t RSpeedControllerPwm::GetRawPosition()
{
    return(0.0);
}


/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetPosition()
{
    return(0.0);
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t RSpeedControllerPwm::GetRawSpeed()
{
	return 0;
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetSpeed()
{
    return(0.0);
}

/******************************************************************************
 *
 ******************************************************************************/
int RSpeedControllerPwm::GetDeviceID()
{
    return(-1);
}

/******************************************************************************
 * Current Limits not supported at this time
 ******************************************************************************/
void RSpeedControllerPwm::SetCurrentLimit(uint32_t amps, uint32_t peak, uint32_t duration )
{
    m_current_limit = amps;
}

/******************************************************************************
 * Current Limits not supported at this time
 ******************************************************************************/
void RSpeedControllerPwm::SetCurrentLimitEnabled(bool enabled)
{
    m_current_limit_enabled = enabled;
}
/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetForwardLimitSwitch (bool normally_open, bool enabled, bool zero_position)
{
	//@TODO implement this
}
/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetReverseLimitSwitch  (bool normally_open, bool enabled, bool zero_position)
{
	//@TODO implement this
}
/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetBusVoltage()
{
    return m_power_panel.GetVoltage();
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetOutputVoltage()
{
    return 0.0;
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetOutputCurrent()
{
    return m_power_panel.GetCurrent(m_fuse);
}

/******************************************************************************
 * Get output duty cycle;  not tested; not sure if this is a voltage or percent
 ******************************************************************************/
double RSpeedControllerPwm::GetMotorOutputPercent()
{
	return m_controller->Get();
}

/******************************************************************************
 *
 ******************************************************************************/
RSpeedController::ControlModeType RSpeedControllerPwm::GetControlMode()
{
    return UNKNOWN_TYPE;
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetP(double p)
{
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetI(double i)
{
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetD(double d)
{
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetF(double f)
{
}

void RSpeedControllerPwm::SetIZone(double i_zone)
{
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetCruiseVelocity(double vel)
{
}

/******************************************************************************
 *
 ******************************************************************************/
void RSpeedControllerPwm::SetAcceleration(double accel)
{
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetP()
{
   return 0.0;
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetI()
{
    return 0.0;
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetD()
{
    return 0.0;
}

/******************************************************************************
 *
 ******************************************************************************/
double RSpeedControllerPwm::GetF()
{
    return 0.0;
}


