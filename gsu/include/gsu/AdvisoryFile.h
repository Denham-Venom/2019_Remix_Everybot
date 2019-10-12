/*******************************************************************************
 *
 * File: AdvisoryFile.h
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
#include <stdio.h>

/*******************************************************************************
 *
 * The AdvisoryFile class will handle all messages by sending them 
 * to the provided file.
 *
 ******************************************************************************/
class AdvisoryFile : public Advisory
{
	public:
		AdvisoryFile(FILE *file_ptr);
		~AdvisoryFile();

		void post(Type type, const char *text);

	private:
        FILE * m_file_ptr;
};
