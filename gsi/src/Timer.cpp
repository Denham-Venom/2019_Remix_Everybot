/*******************************************************************************
 *
 * Timer.cpp
 *	Generic System Interface Timer class
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Timer.h"
#include "gsi/Thread.h"
#include "gsi/Time.h"
#include "gsi/Semaphore.h"

#include <map>
#include <vector>
#include <mutex>
#include <algorithm>

namespace gsi
{

// =============================================================================
// =============================================================================
// ===
// === Timer Manager
// ===
// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 *	The Timer Manager class is a private class that is used to manage Timers.
 *	Only one instance of this class will be created (as needed) to provide a
 *	thread on which Timer callbacks are executed.
 *
 ******************************************************************************/
class TimerManager : public Thread
{
	public:
		static void startTimer(Timer *timer);
		static void stopTimer(Timer *timer);

	protected:
		virtual void run(void);

	private:
		TimerManager(void);
		virtual ~TimerManager(void);
		static TimerManager * instance(void);

		void addTimer(Timer *timer);
		void removeTimer(Timer *timer);

		std::vector<Timer *> m_timers;
		std::vector<Timer *> m_start_timers;
		std::vector<Timer *> m_stop_timers;
		static TimerManager * m_instance;
		static double m_max_sleep;

		Semaphore m_wait_semaphore;
		std::mutex  m_timers_mutex;
};

TimerManager * TimerManager::m_instance = nullptr;
double TimerManager::m_max_sleep = 3.0;

/*******************************************************************************
 *
 * This private constructor will be used to create the only instance of the
 * TimerManager class.
 *
 ******************************************************************************/
TimerManager::TimerManager(void)
	: m_wait_semaphore(0)
{
}

/*******************************************************************************
 *
 * Release any resources used by this instance
 *
 ******************************************************************************/
TimerManager::~TimerManager(void) 
{
}

/*******************************************************************************
 *
 * This method is used by static TimerManager methods to get the only instance
 * of this class. The TimerManager thread will be created the first time
 * this method is called.
 *
 ******************************************************************************/
TimerManager * TimerManager::instance(void)
{
	if (nullptr == m_instance)
	{
		m_instance = new TimerManager();
		m_instance->start();
	}
	return m_instance;
}

/*******************************************************************************
 *
 * This static method adds the specified Timer to the list of Timers managed
 * by the TimerManager and starts the time for that Timer.
 *
 * @param	timer	the Timer to start
 *
 ******************************************************************************/
void TimerManager::startTimer(Timer *timer)
{
	TimerManager::instance()->addTimer(timer);
}

/*******************************************************************************
 *
 * This static method removes the Timer from the TimerManager, this stopping
 * calls to the timers callback
 *
 * @param	timer	the Timer to stop
 *
 ******************************************************************************/
void TimerManager::stopTimer(Timer *timer)
{
	TimerManager::instance()->removeTimer(timer);
}

/*******************************************************************************
 *
 * Each pass through the loop in this method all Timers are checked to see if
 * their callbacks should be called and the time until the next Timer callback
 * should be called is calculated. Then waits for that much time or until the
 * list of Timers changes.
 *
 ******************************************************************************/
void TimerManager::run(void)
{
	while (!isStopRequested())
	{
		double now_time = Time::getTime();
		double next_time = now_time + m_max_sleep;

		m_timers_mutex.lock();
		for(Timer * timer : m_stop_timers)
		{
			std::vector<Timer *>::iterator ittr = std::find(m_timers.begin(), m_timers.end(), timer);
			if (ittr != m_timers.end())
			{
				m_timers.erase(ittr);
			}
		}
		m_stop_timers.clear();

		for(Timer * timer : m_start_timers)
		{
			if (timer->isRunning()) // just in case the timer was stopped before getting here
			{
				m_timers.push_back(timer);
			}
		}
		m_start_timers.clear();
		m_timers_mutex.unlock();

		for(Timer * timer : m_timers)
		{
			if (timer->nextTime() <= now_time)
			{
				timer->executeTimerCallback();
			}

			next_time = std::min(next_time, timer->nextTime());
		}

		// Wait for next time or until another timer is started
		double wait_time = next_time - Time::getTime();
		if (wait_time > 0.0)
		{
			m_wait_semaphore.take(wait_time);
		}
	}
}

/*******************************************************************************
 *
 * Adds the timer to the list of timers to be activated and notifies the
 * TimerManager, the TimerManager run method will transfer the timer to the
 * active list when it is safe to do so.
 *
 * @param	timer	the timer that should be added
 *
 ******************************************************************************/
void TimerManager::addTimer(Timer *timer)
{
	m_timers_mutex.lock();
	m_start_timers.push_back(timer);
	m_timers_mutex.unlock();
	m_wait_semaphore.give();
}

/*******************************************************************************
 *
 * Adds the timer to the list of timers to be deactivated and notifies the
 * TimerManager, the TimerManager run method will remove the timer from the
 * active list when it is safe to do so.
 *
 * @param	timer	the timer to be removed from the list
 *
 ******************************************************************************/
void TimerManager::removeTimer(Timer *timer)
{
	m_timers_mutex.lock();
	m_stop_timers.push_back(timer);
	m_timers_mutex.unlock();
	m_wait_semaphore.give();
}

// =============================================================================
// =============================================================================
// ===
// === Timer
// ===
// =============================================================================
// =============================================================================

/*******************************************************************************
 *
 * @param   handle  an std::function that will be called when the timer expires
 * @param   interval    the number of seconds before the timer expires
 * @param   repeat      true if the timer should repeat, false for one time_t
 *
 ******************************************************************************/
Timer::Timer(std::function<void(void)> handle, double interval, bool repeat)
	: m_handle(handle)
	, m_interval(interval)
	, m_repeat(repeat)
	, m_is_running(false)
	, m_next_time(0.0)
{
}

/*******************************************************************************
 *
 * Release any resources held by this instance, calls stop() to insure that
 * this instance is removed from the TimerManager
 *
 ******************************************************************************/
Timer::~Timer(void)
{
	stop();
}

/*******************************************************************************
 *
 * If this timer is not already running, it will be started.
 *
 * NOTE: This will not reset the timer if it is already running.
 *
 ******************************************************************************/
void Timer::start(void)
{
	if (!m_is_running)
	{
		m_next_time = Time::getTime() + m_interval;
		m_is_running = true;
		TimerManager::startTimer(this);
	}
}

/*******************************************************************************
 *
 * If the timer is running it will be stopped.
 *
 ******************************************************************************/
void Timer::stop(void)
{
	if (m_is_running)
	{
		TimerManager::stopTimer(this);
		m_is_running = false;
	}
}

/*******************************************************************************
 *
 * @return true if the timer is running
 *
 ******************************************************************************/
bool Timer::isRunning(void)
{
	return m_is_running;
}

/*******************************************************************************
 *
 * @return the next time that the Timer's callback will be called
 *
 ******************************************************************************/
double Timer::nextTime(void)
{
	return m_next_time;
}

/*******************************************************************************
 *
 * Execute the Timers callback method.
 *
 ******************************************************************************/
void Timer::executeTimerCallback(void)
{
	if (m_is_running)
	{
		m_handle();

		if (false == m_repeat)
		{
			stop();
		}
		else
		{
			m_next_time += m_interval;
		}
	}
}

} // namespace gsi
