/*******************************************************************************
 *
 * File: Thread.h
 *	Generic System Interface Thread wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <exception>
#include <string>
#include <thread>
#include <functional>

#include <stdint.h>

namespace gsi
{

/*******************************************************************************
 *
 * This class provides an object oriented way to interact with threads.  Any
 * class can extend this class and provide an implementation for the run
 * method to operate in a new thread.  
 *
 * When the start() method is called, the new thread is created and the 
 * run function is called from inside of that new thread.  The run function
 * can either be a re-implementation of the run() method or a std::function
 * that is passed to the public constructor.
 *
 * Subclasses should call isStopRequested() periodically, if stop is requested
 * the subclass should clean up and return from the run function.
 *
 * See the documentation for the constructors for examples.
 *
 ******************************************************************************/
class Thread
{
	public:
		enum ThreadPriority 
		{
			PRIORITY_LOWEST, PRIORITY_LOWER, PRIORITY_LOW, PRIORITY_DEFAULT, 
			PRIORITY_HIGH, PRIORITY_HIGHER, PRIORITY_HIGHEST
		};

		// Note: see protected constructor for use when extending this class
		Thread(std::function<void(void)> run_function, 
			std::string name = "_unnamed_thread_", 
			ThreadPriority priority = PRIORITY_DEFAULT);
		
		virtual ~Thread(void);

		void start(void);
		virtual void requestStop(void);
		virtual void waitForStop(void);

		bool isRunning(void);
		bool isStopRequested(void);

		void setPriority(ThreadPriority priority);
		ThreadPriority getPriority(void);

		std::thread::id getId(void);

		void setName(std::string name);
		std::string getName(void);

		static void sleep(double seconds);

	protected:
		Thread(std::string name = "_unnamed_thread_",
			ThreadPriority priority = PRIORITY_DEFAULT);

		virtual void run(void) {}

	private:
		void runImpl(void);

		std::thread *m_thread;		
		std::string m_thread_name;
		std::thread::id m_thread_id;
		std::function<void(void)> m_run_function;

		ThreadPriority m_thread_priority;
		
        bool m_is_running;
		bool m_is_stop_requested;
};

} // namespace gsi
