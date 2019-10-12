/*******************************************************************************
 *
 * File: PeriodicThread.h
 * 	Generic System Interface Periodic Thread class
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "gsi/Thread.h"

namespace gsi
{

/*******************************************************************************
 *
 * This class extends the Thread class with an implementation of the
 * run() method that periodically calls a doPeriodic() method that must be 
 * implemented by subclasses.
 * 
 * The period is set when this class is created.  Calls to the doPeriodic() method
 * will be made every <period> seconds as long as the previous call to the
 * doPeriodic() method has completed.  The timing of the next call to doPeriodic() is not
 * dependent on how long it takes the doPeriodic() method to finish as long as it
 * finishes before the next call should be made.  Each call is made based on 
 * when the init() method was called, how many times doPeriodic() has been called
 * since init() and the period. If doPeriodic() does not return by the end of
 * the period, periods may be skipped.
 *
 ******************************************************************************/
class PeriodicThread : public Thread
{
	public:
		PeriodicThread(std::string name, double period = 0.01, 
			ThreadPriority priority = PRIORITY_DEFAULT);

		virtual ~PeriodicThread();

		double getPeriod();
		void setPeriod(double period);
		
	protected:
		virtual void run(void);
		virtual void doPeriodic(void) = 0;

	private:
		double thread_next_time;
		double thread_period;
};

} // namespace gsi
