/*******************************************************************************
 *
 * File: AdvisoryStdErr.h
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
 * The AdvisoryStdErr class will print all messages to standard error
 *
 ******************************************************************************/
class AdvisoryStdErr : public Advisory
{
	public:
        AdvisoryStdErr(void);
        ~AdvisoryStdErr(void);

        void post(Type type, const char *text );
};


