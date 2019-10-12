/*******************************************************************************
 *
 * File: Advisory.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/Advisory.h"

const char *Advisory::TypeName[] = {"", "CAUTION: ", "WARNING: ", "FATAL: "};
std::vector<Advisory *> Advisory::m_observers;

/*******************************************************************************
 *
 * The protected default constructor requires a subclass in order to be
 * instantiated.
 *
 ******************************************************************************/
Advisory::Advisory(void)
{
}

/*******************************************************************************
 *
 * The virtual destructor will delete the implementation
 *
 ******************************************************************************/
Advisory::~Advisory(void)
{
	for (Advisory *obs : m_observers)
	{
		delete obs;
	}

	m_observers.clear();
}

/*******************************************************************************
 *
 * Save the subclass implementation that will be used for all future messages.
 * If this is not the first time this method is called, the previous 
 * implementation will be deleted.
 *
 ******************************************************************************/
void Advisory::addObserver(Advisory *implA)
{
	m_observers.push_back(implA);
}

/*******************************************************************************
 *
 * Post a text message to the user
 *
 * This method converts the the format string and argument list into a
 * formatted text string, then passes that string to all current observers.
 *
 * @param type	the type of the message, this may be used by subclasses to
 *				determin when/where/how the message is reported.
 *
 * @param fmt	the format string for the message
 *
 * @param vlist	the variable argument list
 *
 ******************************************************************************/
void Advisory::post_impl(Advisory::Type type, const char *fmt, va_list vlist)
{
	char fmt_buffer[4096];
	vsnprintf(fmt_buffer, 4096, fmt, vlist);

	for (Advisory *obs : m_observers)
	{
		obs->post(type, fmt_buffer);
	}
}

/*******************************************************************************
 *
 * Post an information message to the user
 *
 * @param fmt	the format string for the message, see printf
 *
 * @param ...	any arguments needed by the format string
 *
 ******************************************************************************/
void Advisory::pinfo(const char *fmt, ...)
 {
    va_list argList;

    va_start(argList, fmt);
    post_impl(INFO, fmt, argList);
    va_end(argList); 
 }

/*******************************************************************************
 *
 * Post a caution message to the user, just something the user should 
 * be aware of but doesn't prevent the software from working
 *
 * @param fmt	the format string for the message, see printf
 *
 * @param ...	any arguments needed by the format string
 *
 ******************************************************************************/
void Advisory::pcaution(const char *fmt, ...)
 {
    va_list argList;

    va_start(argList, fmt);
    post_impl(CAUTION, fmt, argList);
    va_end(argList); 
 }

/*******************************************************************************
 *
 * Post a warning message to the user, some error or condition was detected 
 * that may result in reduced functionality of the code
 *
 * @param fmt	the format string for the message, see printf
 *
 * @param ...	any arguments needed by the format string
 *
 ******************************************************************************/
void Advisory::pwarning(const char *fmt, ...)
 {
    va_list argList;

    va_start(argList, fmt);
    post_impl(WARNING, fmt, argList);
    va_end(argList); 
 }

/*******************************************************************************
*
* Post a fatal message to the user, something went wrong and the software is
* about to exit or major functionality is being disabled
*
* @param fmt	the format string for the message, see printf
*
* @param ...	any arguments needed by the format string
*
******************************************************************************/
void Advisory::pfatal(const char *fmt, ...)
 {
    va_list argList;

    va_start(argList, fmt);
    post_impl(FATAL, fmt, argList);
    va_end(argList); 
 }
