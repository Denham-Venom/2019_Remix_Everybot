/*******************************************************************************
 *
 * File: Thread.cpp
 * 	Generic System Interface class for threads
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Thread.h"

#include <stdio.h>
#include <sstream>

namespace gsi
{

/*******************************************************************************
 *
 * This constructor is protected, it is to be used during the construction of
 * a subclass that will run a new thread with the specified options. The
 * subclass should provide an implementation for the run() method.
 *
 * @param	name		The name of the thread.
 *
 * @param	priority	The priority of this thread can be specified with
 *                      one of the defined enumerations
 *
 * Example:
	class MyThread : public gsi::Thread
	{
		public:
			MyThread(int id, std::string name) 
				: Thread(name)
				, m_id(id)
			{}

			void run(void) 
			{ 
				while (!isStopRequested()) 
				{ 
					sleep(1.0); 
					printf("Hello World from %s-%d\n", getName().c_str(), m_id); 
				} 
			}

		private:
			int m_id;
	};

	int main()
	{
		MyThread test(42, "my_test");
		test.start();

		gsi::Thread::sleep(20.0);
	}
 *
 ******************************************************************************/
Thread::Thread(std::string name, Thread::ThreadPriority priority)
	: Thread(std::bind(&Thread::run, this), name, priority)
{
}

/*******************************************************************************
 *
 * This constructor is public, it can be used to create an instance of this
 * base class or a subclass that wishes to use something other than the 
 * run() method as it's execution function. Either way it will execute the
 * function in a thread with the given options.
 *
 * @param   run_function The function that will be run when this thread
 *                       starts.
 *
 * @param	name		The name of the thread.
 *
 * @param	priority	The priority of this thread can be specified, 0 is
 *						highest priority, 255 is lowest priority.  This
 *						defaults to 100 if not specified.
 * Example:
	void f1(void) 
	{ 
		for (int i = 0; i < 10; i++) 
		{ 
			gsi::Thread::sleep(1.0);
			printf("Hello from f1\n");
		} 
	}

	void f2(gsi::Thread **t, int arg2) 
	{ 
		while (!(*t)->isStopRequested()) 
		{ 
			(*t)->sleep(1.0); 
			printf("Hello from f2-%d\n", arg2); 
		} 
	}

	int main()
	{
		gsi::Thread my_f1_thread(std::bind(f1));
		gsi::Thread *my_f2_thread = new gsi::Thread(std::bind(f2, &my_f2_thread, 23));

		my_f1_thread.start();
		my_f2_thread->start();

		gsi::Thread::sleep(20.0);

		delete my_f2_thread;
	}
 *
 *
 ******************************************************************************/
Thread::Thread(std::function<void(void)> run_function,
	std::string name, Thread::ThreadPriority priority)
	: m_run_function(run_function)
{
	m_thread_name = name;
	m_thread_priority = priority;

	m_is_running = false;
	m_is_stop_requested = false;

	m_thread = nullptr;
}

/*******************************************************************************
 *
 * To be safe this destructor will simply put in a request for the thread to
 * stop.  When the run() method returns, the runHandlerImpl() method takes 
 * care of releasing the resources allocated by this class and calls finalize() 
 * to allow subclasses to release resources they allocate in a thread-safe 
 * manner.
 *
 ******************************************************************************/
Thread::~Thread(void)
{
	waitForStop();

	if (m_thread != nullptr)
	{
		delete m_thread;
		m_thread = nullptr;
	}
}

/*******************************************************************************
 *
 * If the thread is not already running, start it.  Starting a thread will clear
 * any previous stop requests.
 *
 ******************************************************************************/
void Thread::start(void)
{
	if (!m_is_running)
	{
		m_is_stop_requested = false;

		if (m_thread != nullptr)
		{
			m_thread->join();
			delete m_thread;
		}

		m_thread = new std::thread(&Thread::runImpl, this);
		setPriority(m_thread_priority);
		setName(m_thread_name);
	}
}

/*******************************************************************************
 *
 * This method will only return after the specified number of seconds 
 * (to the nearest tick) have passed.  The exact ammount of time it takes
 * for this method to return will be impacted by the systems clock resolution.
 *
 * @param	time the amount of time in seconds and fractions of a second that
 *			the calling thread should be delayed.
 *
 ******************************************************************************/
void Thread::sleep(double time)
{
	if (time > 0.0)
	{
		std::this_thread::sleep_for(
			std::chrono::microseconds((uint64_t)(time * 1000000)));
	}
}

/*******************************************************************************
 *
 * Set the priority that this thread should have when it is run.
 *
 * NOTICE:	setting thread priorities does not always work, it is platform
 *			user, and configuration dependent
 *
 * @param	priority	the new priority for this thread
 *
 ******************************************************************************/
void Thread::setPriority(Thread::ThreadPriority priority)
{
	m_thread_priority = priority;
}

/*******************************************************************************
 *
 * NOTICE:	setting thread priorities does not always work, it is platform
 *			user, and configuration dependant
 *
 * @return the priority of this thread
 *
 ******************************************************************************/
Thread::ThreadPriority Thread::getPriority(void)
{
	return m_thread_priority;
}

/*******************************************************************************
 *
 * @return true if there is currently a thread running for this class
 *
 ******************************************************************************/
bool Thread::isRunning(void)
{
	return m_is_running;
}

/*******************************************************************************
*
* This sets a flag that indicates the thread should stop the next time it
* can safely be stopped.  It is up to the subclasses to honor this request
* by using the isStopRequested() method periodically.
*
******************************************************************************/
void Thread::requestStop(void)
{
	m_is_stop_requested = true;
}

/*******************************************************************************
*
* This sets a flag that indicates the thread should stop the next time it
* can safely be stopped.  Then it waits for the thread to indicate that
* it is no longer running before returning;
*
******************************************************************************/
void Thread::waitForStop(void)
{
	if (m_is_running)
	{
		requestStop();
	}

	if (m_thread != nullptr)
	{
		m_thread->join();
	}
}

/*******************************************************************************
 *
 * @return true if stop has been called since the last time start has been
 *			called.
 *
 ******************************************************************************/
bool Thread::isStopRequested(void)
{
	return m_is_stop_requested;
}

/*******************************************************************************
 *
 * @return  The ID of the thread running for this class, ERROR if there is not
 * 			currently a thread running.
 *
 ******************************************************************************/
std::thread::id Thread::getId()
{
	return m_thread_id;
}

/*******************************************************************************
 *
 * Set the thread name so it is available when debugging
 *
 ******************************************************************************/
void Thread::setName(std::string name)
{
	m_thread_name = name;
	if (m_thread != nullptr)
	{
	}
}

/*******************************************************************************
 *
 * @return the name of this thread.
 *
 ******************************************************************************/
std::string Thread::getName(void)
{
	return m_thread_name;
}

/*******************************************************************************
 *
 * This method is what actually gets run in the thread. It calls the
 * initialize(), run(), and finalize() methods on the thread, then cleans up
 * the underlying thread implementation.
 *
 ******************************************************************************/
void Thread::runImpl(void)
{
	m_thread_id = std::this_thread::get_id();
	m_is_running = true;
	if ( ! m_is_stop_requested)
	{
		try
		{
			m_run_function();
		}
		catch (std::exception& ex)
		{
			fprintf(stderr, "Thread::runHandle - unhandled exception during running of thread %s -- %s\n",
				getName().c_str(), ex.what());
		}
		catch (...)
		{
			fprintf(stderr, "Thread::runHandle - unhandled exception during running of thread %s\n",
				getName().c_str());
		}
	}
	m_is_running = false;
}

} // namespace gsi
