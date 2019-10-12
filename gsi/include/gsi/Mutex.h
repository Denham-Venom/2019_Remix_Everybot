/*******************************************************************************
 *
 * File: Mutex.h
 *
 * 	This file contains six Mutex related classes. 
 *
 *  With the availability of the C++11 mutex classes there is no longer a need 
 *  for this collection of classes, but to maintain the gsi style and provide 
 *  isolation from future changes this collection provides a set of very thin 
 *  wrappers for the C++11 standard Mutex classes. 
 *
 *  Mutex               - a base class that provides a common interface
 *  ExclusiveMutex      - a thin wrapper for std::mutex
 *  RecursiveMutex      - a thin wrapper for std::recursive_mutex
 *  ExclusiveTimedMutex - a thin wrapper for std::timed_mutex
 *  RecursiveTimedMutex - a thin wrapper for std::recursive_timed_mutex
 *  MutexScopeLock      - functionally the same as std::lock_guard
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <mutex>

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to a mutex.
 *
 ******************************************************************************/
class Mutex
{
	public:
		virtual ~Mutex(void) {}

		virtual void lock(void) = 0; 
		virtual void unlock(void) = 0;					 
		virtual bool tryLock(double seconds = 0.0) = 0;

	protected:
		Mutex(void) {}
};

/*******************************************************************************
 *
 * An exlusive mutex can be used to protect a set of resources such
 * that nothing else (not even additional calls from the same thread) can 
 * access those resources.
 *
 ******************************************************************************/
class ExclusiveMutex : public Mutex
{
	public:
		ExclusiveMutex(void) : Mutex()            {}
		virtual ~ExclusiveMutex(void)             {}

		inline void lock(void)                    { m_mutex.lock(); }
		inline void unlock(void)                  { m_mutex.unlock(); }
                inline bool tryLock(double seconds = 0.0) { (void)seconds;  return m_mutex.try_lock(); }

	private:
		std::mutex m_mutex;
};

/*******************************************************************************
 *
 * A recursive mutex can be used to protect a set of resources such
 * that no other thread can access those resources, additional locks from the
 * same thread are allowed but each lock must have a matching unlock.
 *
 ******************************************************************************/
class RecursiveMutex : public Mutex
{
	public:
		RecursiveMutex(void) : Mutex()            {}
		virtual ~RecursiveMutex(void)             {}

		inline void lock(void)                    { m_mutex.lock(); }
		inline void unlock(void)                  { m_mutex.unlock(); }
                inline bool tryLock(double seconds = 0.0) { (void)seconds; return m_mutex.try_lock(); }

	private:
		std::recursive_mutex m_mutex;
};

/*******************************************************************************
 *
 * An exlusive timed mutex can be used to protect a set of resources such
 * that nothing else (not even additional calls from the same thread) can
 * access those resources. The tryLock method can be used to wait a specified
 * amount of time while attempting to access the resources.
 *
 ******************************************************************************/
class ExclusiveTimedMutex : public Mutex
{
	public:
		ExclusiveTimedMutex(void) : Mutex()       {}
		virtual ~ExclusiveTimedMutex(void)        {}

		inline void lock(void)                    { m_mutex.lock(); }
		inline void unlock(void)                  { m_mutex.unlock(); }
		inline bool tryLock(double seconds = 0.0) 
		{ 
			return m_mutex.try_lock_for(
				std::chrono::microseconds((uint64_t)(seconds * 1000000))); 
		}

	private:
		std::timed_mutex m_mutex;
};

/*******************************************************************************
 *
 * A recursive mutex can be used to protect a set of resources such
 * that no other thread can access those resources, additional locks from the
 * same thread are allowed but each lock must have a matching unlock. The tryLock 
 * method can be used to wait a specified amount of time while attempting to 
 * access the resources.
 *
 ******************************************************************************/
class RecursiveTimedMutex : public Mutex
{
	public:
		RecursiveTimedMutex(void) : Mutex()       {}
		virtual ~RecursiveTimedMutex(void)        {}

		inline void lock(void)                    { m_mutex.lock(); }
		inline void unlock(void)                  { m_mutex.unlock(); }
		inline bool tryLock(double seconds = 0.0)
		{
			return m_mutex.try_lock_for(
				std::chrono::microseconds((uint64_t)(seconds * 1000000)));
		}

	private:
		std::recursive_timed_mutex m_mutex;
};

/*******************************************************************************
 *
 * Create an instance of this class to lock the provided Mutex until the
 * instance of this class looses scope.
 *
 ******************************************************************************/
class MutexScopeLock
{
	public:
		MutexScopeLock(Mutex &m) : mtex(m) { mtex.lock(); }
		~MutexScopeLock(void)    { mtex.unlock(); }

	private:
		Mutex &mtex;
};

} // namespace gsi
