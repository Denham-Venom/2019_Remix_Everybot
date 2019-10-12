/*******************************************************************************
 *
 * File: PeriodicThread.cpp
 * 	Generic System Interface class for periodic threads
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/PeriodicThread.h"
#include "gsi/Time.h"

#include <math.h>

static const double MIN_SLEEP_TIME = 0.001;

namespace gsi
{

/*******************************************************************************
 *
 * Constructor for PeriodicTThread
 * 
 * The constructor initializes the instance variables for the task.
 *
 ******************************************************************************/
PeriodicThread::PeriodicThread(std::string name, double period, ThreadPriority priority) 
	: Thread(name, priority)
{
	thread_period = fabs(period);
	thread_next_time = 0.0;
}

/*******************************************************************************
 *
 *  Free the resources for this class.
 *
 ******************************************************************************/
PeriodicThread::~PeriodicThread()
{
}

/*******************************************************************************
 *
 * @return the period in seconds
 *
 ******************************************************************************/
double PeriodicThread::getPeriod()
{
	return thread_period;
}

/*******************************************************************************
 *
 * Set the period of this thread.  If the thread is already running, the new
 * period will not have an impact until the current cycle completes.
 * 
 * @param	period	the new period in seconds
 *
 ******************************************************************************/
void PeriodicThread::setPeriod(double period)
{
	thread_period = fabs(period);
}

/*******************************************************************************
 *
 * This method implements the base classes run method to periodically call
 * the doPeriodic() method that must be implemented by subclasses.
 *
 ******************************************************************************/
void PeriodicThread::run(void)
{
	double wait_time = thread_period;
	thread_next_time = Time::getTime();
	
	while( ! isStopRequested() )
	{
		doPeriodic();

		thread_next_time += thread_period;
		wait_time = thread_next_time - Time::getTime();

		if (wait_time > thread_period)  // something went wrong so reset
		{
			thread_next_time = Time::getTime() + thread_period;
			wait_time = thread_period;
			sleep(wait_time);
		}
		else if (wait_time >= MIN_SLEEP_TIME)
		{
			sleep(wait_time);
		}
		else // a period took too long, try to catch up
		{
			if (wait_time < (thread_period * -2.0)) // getting too far behind, skip some periods
			{
				thread_next_time += abs((int)(wait_time/thread_period)) * thread_period;
			}

			sleep(MIN_SLEEP_TIME);
		}
	}
}

} // namespace gsi
