/*******************************************************************************
 *
 * File: PeriodicControl.h
 * 
 * This file contains the definition of a base class for controls that need
 * to run in a separate thread.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <string>
#include <stdint.h>

#include "RobonautsControls/ControlThread.h"
#include "gsi/Mutex.h"

/*******************************************************************************
 *
 * This base class extends the Thread class and is designed to be used with
 * IterativeRobot, ContinuousRobot, or PeriodicRobot to provide a template for
 * control objects that require accurate control of timing and thread safe
 * operations.
 *
 * The base class provides several init methods that must be implemented by a
 * subclass. These methods should get called as the robot transitions
 * between game phases. The required doPeriodic() method will be called every
 * period, care should be taken to make sure it can complete it's work in
 * the period set for the control object.
 *
 * The base class also provides a single mutex to help with interaction
 * between threads.
 *
 ******************************************************************************/
class PeriodicControl : public ControlThread
{
	public:
		PeriodicControl(std::string name);
		virtual ~PeriodicControl(void);

		void setPeriod(double period);
		double getPeriod(void);

		uint16_t getCyclesSincePublish(void);

		virtual void run(void);

	protected:
		virtual void doPeriodic(void) = 0;

	private:
		double m_control_period;

		uint16_t m_control_cycles_since_publish; // for health monitoring
};
