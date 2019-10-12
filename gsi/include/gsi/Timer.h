/*******************************************************************************
 *
 * File: Timer.h
 *	Generic System Interface Timer class
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <functional>

namespace gsi
{

/*******************************************************************************
 *
 *	This class provides a way for user provided methods to be called at a
 *	later time or periodically.
 *
 *	WARNING: The provided method is called on an internal TimerManager thread
 *	that is shared by all timers so the method should be designed to
 *	use minimal CPU resources. The called method should set a flag or
 *	do a semaphore give, or some other thing that allows CPU intensive or IO
 *	operations to be done on a different thread.
 *
 *	See gsi::PeriodicThread for another option.
 *
 ******************************************************************************/
class Timer
{
	public:
        Timer(std::function<void(void)> handle, double interval, bool repeat=true);
        virtual ~Timer(void);

        void start(void);
        void stop(void);

        bool isRunning(void);
		double nextTime(void);

    private:
        void executeTimerCallback(void);

        std::function<void(void)> m_handle;
        double m_interval;
        bool m_repeat;
        bool m_is_running;
		double m_next_time;

	friend class TimerManager;
};

} // namespace gsi
