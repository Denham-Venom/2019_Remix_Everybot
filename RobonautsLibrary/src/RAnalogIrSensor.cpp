/*----------------------------------------------------------------------------*/
/* Robonauts abstraction of IR analog sensor                                  */
/* Initially used during 2018 for line sensors                                */
/*----------------------------------------------------------------------------*/

#include "RobonautsLibrary/RAnalogIrSensor.h"

using namespace frc;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
RAnalogIrSensor::RAnalogIrSensor(int channel) : AnalogInput(channel)
{
	m_threshold = DEFAULT_THRESHOLD_VALUE;
	m_threshold_type = RAnalogIrSensor::kLessThan;
	m_default_value = DEFAULT_IR_VALUE;
	m_latch_cycles = DEFAULT_LATCH_CYLES;
	m_latch_count = 0;
	m_value = 0;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
RAnalogIrSensor::~RAnalogIrSensor()
{

}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void RAnalogIrSensor::setThreshold(int threshold)
{
	m_threshold = threshold;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void RAnalogIrSensor::setLatchCycles(int cycles)
{
	m_latch_cycles = cycles;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void RAnalogIrSensor::setThresholdType(RAnalogIrSensor::ThresholdType type)
{
	m_threshold_type = type;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
bool RAnalogIrSensor::onLine()
{
	bool ret = false;

	if(m_latch_count > 0)
	{
		ret = true;
	}
	return ret;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int RAnalogIrSensor::update()
{
	int value = GetValue();

	if(m_threshold_type == RAnalogIrSensor::kGreaterThan && value > m_threshold)
	{
		m_latch_count = m_latch_cycles;
	}
	else if(m_threshold_type == RAnalogIrSensor::kGreaterThan && m_latch_count > 0)
	{
		m_latch_count--;
	}
	else if(m_threshold_type == RAnalogIrSensor::kLessThan && value < m_threshold)
	{
		m_latch_count = m_latch_cycles;
	}
	else if(m_threshold_type == RAnalogIrSensor::kLessThan && m_latch_count > 0)
	{
		m_latch_count--;
	}
	return value;
}
