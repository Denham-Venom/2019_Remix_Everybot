/*******************************************************************************
 *
 * File: RDblSolenoid.h - Double solenoid
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
#include "Solenoid.h"
#else
#include "frc/Solenoid.h"  //WPI
#endif

using namespace frc;

/*******************************************************************************
 *
 * This class provides an interface for getting switch inputs, it works for
 * switches that are wired normally open and normally closed. When the class
 * is configured for the appropriate type of switch, the Get method will
 * return true when the switch is pressed.
 * 
 ******************************************************************************/
class RDblSolenoid
{
	public:
		RDblSolenoid(uint32_t module, uint32_t channel_a, uint32_t channel_b);
		virtual ~RDblSolenoid(void);

		virtual void set(bool val);

	private:
		Solenoid* solenoid_a;
		Solenoid* solenoid_b;
};
