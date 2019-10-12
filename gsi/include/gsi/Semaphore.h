/*******************************************************************************
 *
 * File: Semaphore.h
 * 	Generic System Interface Semaphore wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdint.h>
#include <mutex>
#include <condition_variable>

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to semaphores.
 *
 ******************************************************************************/
class Semaphore
{
	public:
		static const uint16_t DEFAULT_MAX_VALUE = 65535;

		Semaphore(uint16_t init_value = 1, uint16_t max_value = DEFAULT_MAX_VALUE);
		~Semaphore(void);

		bool take(double timeout=-1.0); // same as wait
		void give(void);				// same as post
		
		uint16_t getCurrentValue(void);

	private:
		uint16_t m_current_value;
		uint16_t m_max_value;

		std::mutex m_mutex;
		std::condition_variable m_condition;
};

} // namespace gsi
