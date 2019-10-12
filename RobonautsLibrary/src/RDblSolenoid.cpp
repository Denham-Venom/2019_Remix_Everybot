/*******************************************************************************
 *
 * File: RDblSolenoid.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RDblSolenoid.h"

using namespace frc;

/******************************************************************************
 * 
 * Create an interface to an APS plugged into the specified channel.
 * 
 * @param	channel	the channel or port that the pot is in
 * 
 ******************************************************************************/
RDblSolenoid::RDblSolenoid(uint32_t module, uint32_t channel_a, uint32_t channel_b)
{
	solenoid_a = new Solenoid(module, channel_a);
	solenoid_b = new Solenoid(module, channel_b);
}

/******************************************************************************
 *
 ******************************************************************************/
RDblSolenoid::~RDblSolenoid(void)
{
	if(nullptr != solenoid_a)
	{
		delete solenoid_a;
	}
	if(nullptr != solenoid_b)
	{
		delete solenoid_b;
	}
}

/******************************************************************************
 * 
 ******************************************************************************/
void RDblSolenoid::set(bool val)
{
	solenoid_a->Set(val);
	solenoid_b->Set(!val);
}
