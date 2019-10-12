/*******************************************************************************
 *
 * File: ControlThread.h
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

#include "gsi/Thread.h"
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
class ControlThread : public gsi::Thread
{
	public:
		enum ControlPhase
		{
			INIT, DISABLED, AUTON, TELEOP, TEST
		};

		ControlThread(std::string name,
			gsi::Thread::ThreadPriority priority = gsi::Thread::PRIORITY_DEFAULT);
		virtual ~ControlThread(void);

		void doControlInit(void);
		void doDisabledInit(void);
		void doAutonomousInit(void);
		void doTeleopInit(void);
		void doTestInit(void);
		void doPublish(void);

		ControlPhase getPhase(void);
		double getPhaseElapsedTime(void);

		virtual void run(void) = 0;           // subclasses must override this method

	protected:
		virtual void controlInit(void) {}     // subclasses should override this method to provide functionality
		virtual void disabledInit(void) {}    // subclasses should override this method to provide functionality
		virtual void autonomousInit(void) {}  // subclasses should override this method to provide functionality
		virtual void teleopInit(void) {}      // subclasses should override this method to provide functionality
		virtual void testInit(void) {}        // subclasses should override this method to provide functionality
		virtual void publish(void) {}         // subclasses should override this method to provide functionality

		gsi::Mutex& getLock(void);
        void phaseInit(ControlPhase phase);

	private:
		double control_phase_time;
		ControlPhase control_phase;

		gsi::ExclusiveMutex control_lock;
};
