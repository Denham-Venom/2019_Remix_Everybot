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
#include "gsu/AdvisoryStdErr.h"

/*******************************************************************************
 *
 * The private constructor will prevent instances of this class from being
 * created out of context, AdvisoryStdErr::init() should be used instead.
 *
 ******************************************************************************/
AdvisoryStdErr::AdvisoryStdErr(void) : Advisory()
{
}

/*******************************************************************************
 *
 * The virtual destructor allows for all clean up
 *
 ******************************************************************************/
AdvisoryStdErr::~AdvisoryStdErr(void)
{
}

/*******************************************************************************
 *
 * This method handles all of the posted messages by sending the message to
 * the standard output.
 *
 ******************************************************************************/
void AdvisoryStdErr::post(Advisory::Type type, const char *text )
{
    fprintf(stderr,"%s%s\n", TypeName[type], text);
}
