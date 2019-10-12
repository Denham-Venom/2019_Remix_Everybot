/*******************************************************************************
 *
 * File: AdvisoryStdOut.h
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "gsu/Advisory.h"

/*******************************************************************************
 *
 * The AdvisoryStdOut class will print all messages to standard out
 *
 ******************************************************************************/
class AdvisoryStdOut : public Advisory
{
	public:
		AdvisoryStdOut(void);
		~AdvisoryStdOut(void);

        void post(Type type, const char *text );
};


