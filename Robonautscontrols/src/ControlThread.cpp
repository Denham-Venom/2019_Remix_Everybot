/*******************************************************************************
 *
 * File: ControlThread.cpp
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

#include "RobonautsControls/ControlThread.h"

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
ControlThread::ControlThread(string name, gsi::Thread::ThreadPriority priority)
	: gsi::Thread(name, priority)
{
	phaseInit(INIT);
}

/*******************************************************************************
 *
 *  Free the resources for this class.
 *
 ******************************************************************************/
ControlThread::~ControlThread()
{
}

/*******************************************************************************
 *
 * @return a reference to the locking semaphore used by instances of this
 * class.
 *
 ******************************************************************************/
gsi::Mutex& ControlThread::getLock()
{
	return control_lock;
}

/*******************************************************************************
 *
 * @return the phase that this control is currently in
 *
 ******************************************************************************/
ControlThread::ControlPhase ControlThread::getPhase()
{
	return control_phase;
}

/*******************************************************************************
 *
 * @return how long (in seconds) this object has been in the current phase
 *
 ******************************************************************************/
double ControlThread::getPhaseElapsedTime()
{
	return gsi::Time::getTime() - control_phase_time;
}

/*******************************************************************************
 *
 * This method is called internally to initialize variables that are used
 * to track phase transitions and the amount of time spent in a phase.
 *
 ******************************************************************************/
void ControlThread::phaseInit(ControlPhase phase)
{
	control_phase = phase;
	control_phase_time = gsi::Time::getTime();
}

/*******************************************************************************
 *
 * This method should be called shortly after the object is created, normally
 * during robot initialization, it will acquire the controls lock before
 * calling the subclasses controlInit() method.
 *
 ******************************************************************************/
void ControlThread::doControlInit()
{
	try
	{
		phaseInit(INIT);
		controlInit();
	}
	catch (...)
	{
		Advisory::pwarning("Exception caught in PeriodicControl::doControlInit -- %s", getName().c_str());
	}
}

/*******************************************************************************
 *
 * This method should be called when the robot is entering the disabled mode,
 * it will acquire the controls lock before calling the subclasses
 * disableInit() method.
 *
 ******************************************************************************/
void ControlThread::doDisabledInit()
{
	try
	{
		phaseInit(DISABLED);
		disabledInit();
	}
	catch (...) 
	{
		Advisory::pwarning("Exception caught in PeriodicControl::doDisabledInit -- %s", getName().c_str());
	}
}

/*******************************************************************************
 *
 * This method should be called when the robot is entering the autonomous mode,
 * it will acquire the controls lock before calling the subclasses
 * autonomousInit() method.
 *
 ******************************************************************************/
void ControlThread::doAutonomousInit()
{
	try
	{
		phaseInit(AUTON);
		// 5 SECONDS HAPPENING HERE
		autonomousInit();
	}
	catch (...) 
	{
		Advisory::pwarning("Exception caught in PeriodicControl::doAutonomousInit -- %s", getName().c_str());
	}
}

/*******************************************************************************
 *
 * This method should be called when the robot is entering the teleop mode,
 * it will acquire the controls lock before calling the subclasses
 * teleopInit() method.
 *
 ******************************************************************************/
void ControlThread::doTeleopInit()
{
	try
	{
		phaseInit(TELEOP);
		// THE 5 SECOND DELAY HAPPENS HERE
		teleopInit();
	}
	catch(...)
	{
		Advisory::pwarning("Exception caught in PeriodicControl::doTeleopInit -- %s", getName().c_str());
	}
}

/*******************************************************************************
 *
 * This method should be called when the robot is entering the test mode,
 * it will acquire the controls lock before calling the subclasses
 * teleopInit() method.
 *
 ******************************************************************************/
void ControlThread::doTestInit()
{
	try
	{
		phaseInit(TEST);
		testInit();
	}
	catch (...) 
	{
		Advisory::pwarning("Exception caught in PeriodicControl::doTestInit -- %s", getName().c_str());
	}
}

/*******************************************************************************
 *
 * This method should be called when the robot is entering the test mode,
 * it will acquire the controls lock before calling the subclasses
 * teleopInit() method.
 *
 ******************************************************************************/
void ControlThread::doPublish()
{
	try
	{
		publish();
	}
	catch (...)
	{
		Advisory::pwarning("Exception caught in PeriodicControl::doPublish -- %s", getName().c_str());
	}
}
