/*******************************************************************************
 *
 * File: AdvisoryFile.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/AdvisoryFile.h"

/*******************************************************************************
 *
 * Create an instance of this class
 *
 ******************************************************************************/
AdvisoryFile::AdvisoryFile(FILE *file_ptr) 
	: Advisory()
    , m_file_ptr(file_ptr)
{
}

/*******************************************************************************
 *
 * The virtual desturctor allows for all clean up
 *
 ******************************************************************************/
AdvisoryFile::~AdvisoryFile(void)
{
    if (nullptr != m_file_ptr)
    {
        fclose(m_file_ptr);
        m_file_ptr = nullptr;
    }
}

/*******************************************************************************
 *
 * This method handles all of the posted messages by sending the message to
 * a file
 *
 ******************************************************************************/
void AdvisoryFile::post(Advisory::Type type, const char *text )
{
	if (nullptr != m_file_ptr)
	{
		fprintf(m_file_ptr, "%s%s\n", TypeName[type], text);
		fflush(m_file_ptr);
	}
}
