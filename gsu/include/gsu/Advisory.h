/*******************************************************************************
 *
 * File: Advisory.h
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <vector>

/*******************************************************************************
 *
 * The Advisory class provides an interface for sending messages to the
 * user. This class by itself does nothing but provide an interface. In order
 * for the messages to get to the user, one of the sub classes must be 
 * initialized to provide handling of the messages. Additional subclasses
 * can be created to provide different options for handling the messages.
 *
 ******************************************************************************/
class Advisory 
{
	public:
		typedef enum {INFO, CAUTION, WARNING, FATAL} Type;

        static void pinfo(const char *fmt, ...);
        static void pcaution(const char *fmt, ...);
        static void pwarning(const char *fmt, ...);
        static void pfatal(const char *fmt, ...);

        static const char *TypeName[];

		static void addObserver(Advisory *obs);
		
		template <class T> static T * getInstance(void);

		virtual void post(Type type, const char *text) = 0;

	protected:
		Advisory(void);
		virtual ~Advisory(void);

	private:
		static std::vector<Advisory *> m_observers;
		static void post_impl(Type type, const char *fmt, va_list vlist);
};


/*******************************************************************************
*
* @return the first observer that is an instance of the templated type
*
******************************************************************************/
template<class T> T * Advisory::getInstance(void)
{
	T *instance;

	for (Advisory *obs : m_observers)
	{
		instance = dynamic_cast<T *>(obs);
		if (nullptr != instance)
		{
			return instance;
		}
	}

	return nullptr;
}
