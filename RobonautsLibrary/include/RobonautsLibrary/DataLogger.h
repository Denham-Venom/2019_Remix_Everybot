/*******************************************************************************
 *
 * File: DataLogger.h
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 * 
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <gsu/SegmentedFile.h>

/*******************************************************************************
 *
 * This class contains a generic interface for creating and managing log
 * files on the robot.
 *
 ******************************************************************************/
class DataLogger
{
	public:
        DataLogger(std::string path, std::string name, std::string type, uint32_t max_count, bool enabled=true);
		~DataLogger(void);

		void openSegment(void);

        void setEnabled(bool enabled);
		bool isEnabled(void);
		int32_t getCurrentIndex(void);

		void log(const char* fmt, ...);
		void flush();
		void close();
		
	private:
		bool m_enabled;
		gsu::SegmentedFile m_segmented_file;

		FILE *m_file;
};
