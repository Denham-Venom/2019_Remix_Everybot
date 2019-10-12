/*******************************************************************************
 *
 * File: Semaphore.cpp
 * 		Generic System Interface Semaphore wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Semaphore.h"

#include "gsi/Exception.h"

#include <stdio.h>
#include <string.h>

namespace gsi
{

/*******************************************************************************
 *
 * Create a semaphore with the specified initial value
 *
 * @param 	value	the initial value of the semaphore
 *
 ******************************************************************************/
Semaphore::Semaphore(uint16_t init_value, uint16_t max_value)
{
	m_current_value = init_value;
	m_max_value = max_value;

	if (m_max_value == 0)
	{
		if (m_current_value > 0)
		{
			m_max_value = m_current_value;
		}
		else
		{
			m_max_value = DEFAULT_MAX_VALUE;
		}
	}

	if (m_current_value > m_max_value)
	{
		m_current_value = m_max_value;
	}
}

/*******************************************************************************
 *
 * Release any resources used by this instance
 *
 ******************************************************************************/
Semaphore::~Semaphore(void)
{
}

/*******************************************************************************
 *
 * @return the current value of the semephore
 *
 ******************************************************************************/
uint16_t Semaphore::getCurrentValue(void)
{
	return m_current_value;
}

/*******************************************************************************
 *
 * Take or wait for a resource for the specified amount of time. If the resource
 * is available, the take method will return immediately, if it is not available
 * the caller will block until it becomes available or until the specified
 * timeout has passed.
 *
 * @param	timeout	the number of seconds this should wait for the semaphore,
 * 			if negative, wait forever if needed, default value = -1
 *
 * @return	true if the take was successful, false if it timed out. 
 *
 ******************************************************************************/
bool Semaphore::take(double timeout)
{
	bool return_value = false;

	std::unique_lock<std::mutex> lock(m_mutex);
	if (timeout < 0.0)
	{
		// wait while m_current_value is 0
		m_condition.wait(lock, [this](){ return m_current_value != 0; });
	}
	else
	{
		// wait for timeout or while count is 0
		m_condition.wait_for(lock, 
			std::chrono::microseconds((uint64_t)(timeout * 1000000)), 
			[this](){ return m_current_value != 0; });
	}
	
	if (m_current_value > 0)
	{
		return_value = true;
		m_current_value--;
	}

	lock.unlock();

	return return_value;
}

/*******************************************************************************
 *
 * Give or release one resource that can be used by another thread. This
 * increments the semaphore count by one. If another thread is attempting to
 * take (waiting for) this resource, that thread will be allowed to continue
 * with it's execution.
 *
 ******************************************************************************/
void Semaphore::give()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_current_value < m_max_value)
	{
		m_current_value++;
	}
	m_condition.notify_one();
	lock.unlock();
}

} // namespace gsi
