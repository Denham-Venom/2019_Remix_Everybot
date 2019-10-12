/*******************************************************************************
 *
 * File: PeriodicControl.cpp
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
#include <stdio.h>
#include <math.h>

#include "gsi/Time.h"

#include "gsu/Advisory.h"

#include "RobonautsControls/PeriodicControl.h"

static const double MIN_SLEEP_TIME = 0.001;

using namespace std;

/*******************************************************************************
 *
 * This constructor for PeriodicControl initializes the instance variables for
 * the control.
 *
 * @param name		The name of the control, also used as the name of the
 * 					parent Thread and Task
 *
 * @param period	The time between calls to update() in seconds.
 * 
 * @param priority	The optional priority of the task running this control,
 * 					if not specified Task::kDefaultPriority will be used.
 *
 ******************************************************************************/
PeriodicControl::PeriodicControl(string name) :
	ControlThread(name)
{
	m_control_period = 0.01;
	m_control_cycles_since_publish = 0;
}

/*******************************************************************************
 *
 *  Free the resources for this class.
 *
 ******************************************************************************/
PeriodicControl::~PeriodicControl()
{
}

/*******************************************************************************
 *
 * @param period the period of this control
 *
 ******************************************************************************/
void PeriodicControl::setPeriod(double period)
{
    if (period >= MIN_SLEEP_TIME)
    {
        m_control_period = period;
    }
}

/*******************************************************************************
 *
 * @return the period of this control
 *
 ******************************************************************************/
double PeriodicControl::getPeriod()
{
    return m_control_period;
}

/*******************************************************************************
 *
 * This method can be used to help monitor the status of the thread, the
 * returned value should be dependent on the period of this thread and the
 * period of the doPublish call.
 *
 * @return the number of times doPeriodic returned since the last time the
 * publish method was called
 *
 ******************************************************************************/
uint16_t PeriodicControl::getCyclesSincePublish(void)
{
	return m_control_cycles_since_publish;
}

/*******************************************************************************
 *
 * This method implements the required Thread run() method.  It will enter
 * a loop that calls the subclasses update() method, then waits until the
 * next time the threads update() method should be called.
 *
 * The controls lock is acquired before and released after each call to
 * update() to help insure other threads do not change critical values
 * while the update() method is using them.
 *
 ******************************************************************************/
void PeriodicControl::run(void)
{
	double next_period_start_time = gsi::Time::getTime();
	double wait_time = m_control_period;

	while (!isStopRequested())
	{
		try
		{
			try
			{
				doPeriodic();
			}
			catch(...)
			{
				Advisory::pwarning("caught in PeriodicControl::doPeriodic -- %s", getName().c_str());
			}
			
			//
			// Wait until the next update cycle should begin
			//
			m_control_cycles_since_publish++;
			next_period_start_time += m_control_period;
			wait_time = next_period_start_time - gsi::Time::getTime();

			if (fabs(wait_time) > (5 * m_control_period))
			{
				// if the wait time is too far off in either direction reset
				Advisory::pinfo("PeriodicControl::%s::run -- bad wait time of %f, waiting for period",
					getName().c_str(), wait_time);
				
				next_period_start_time = gsi::Time::getTime() + m_control_period;
				wait_time = m_control_period;
			}
			else if (wait_time < MIN_SLEEP_TIME)
			{
				wait_time = MIN_SLEEP_TIME;
			}

			gsi::Thread::sleep(wait_time);
		}
		catch (...)
		{
			Advisory::pwarning("caught in PeriodicControl::run -- %s", getName().c_str());
		}
	}
}
