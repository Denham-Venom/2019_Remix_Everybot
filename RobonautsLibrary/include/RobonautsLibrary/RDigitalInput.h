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

#ifdef _2018
#include "DigitalInput.h"
#else
#include "frc/DigitalInput.h"
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
class RDigitalInput : public DigitalInput
{
	public:
		enum InputMode
		{
			INPUT_NORMALLY_OPEN = 0,
			INPUT_NORMALLY_CLOSED
		};

		RDigitalInput(uint32_t channel);
		~RDigitalInput(void);

		virtual bool get(void) const;

		void setMode(InputMode mode);
		InputMode getMode(void) const;

	private:
		bool m_normally_open;
};
