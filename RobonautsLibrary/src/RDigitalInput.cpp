/*******************************************************************************
 *
 * File: RDigitalInput.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RDigitalInput.h"

using namespace frc;

/******************************************************************************
 * 
 * Create an interface to an APS plugged into the specified channel.
 * 
 * @param	channel	the channel or port that the pot is in
 * 
 ******************************************************************************/
RDigitalInput::RDigitalInput(uint32_t a_channel)
	: DigitalInput(a_channel)
{
	m_normally_open = true;
}

/******************************************************************************
 *
 ******************************************************************************/
RDigitalInput::~RDigitalInput(void)
{
}

/******************************************************************************
 * 
 * @return  true if the switch is pressed
 * 
 ******************************************************************************/
bool RDigitalInput::get(void) const
{
	return(DigitalInput::Get() == m_normally_open);
}

/******************************************************************************
 * 
 * @param	a_mode	the new input mode, default is INPUT_NORMALLY_OPEN
 * 
 ******************************************************************************/
void RDigitalInput::setMode(RDigitalInput::InputMode a_mode)
{
	m_normally_open = (INPUT_NORMALLY_OPEN == a_mode);
}

/******************************************************************************
 * 
 * @return	the current switch mode
 * 
 ******************************************************************************/
RDigitalInput::InputMode RDigitalInput::getMode(void) const
{
	return m_normally_open ? INPUT_NORMALLY_OPEN : INPUT_NORMALLY_CLOSED;
}
