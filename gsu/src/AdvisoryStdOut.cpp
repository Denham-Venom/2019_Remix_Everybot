/*******************************************************************************
 *
 * File: AdvisoryStdOut.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/AdvisoryStdOut.h"

/*******************************************************************************
 *
 * The private constructor will prevent instances of this class from being
 * created out of context, AdvisoryStdOut::init() should be used instead.
 *
 ******************************************************************************/
AdvisoryStdOut::AdvisoryStdOut(void) : Advisory()
{
}

/*******************************************************************************
 *
 * The virtual destructor allows for all clean up
 *
 ******************************************************************************/
AdvisoryStdOut::~AdvisoryStdOut(void)
{
}

/*******************************************************************************
 *
 * This method handles all of the posted messages by sending the message to
 * the standard output.
 *
 ******************************************************************************/
void AdvisoryStdOut::post(Advisory::Type type, const char *text )
{
	printf("%s%s\n", TypeName[type], text);
}
