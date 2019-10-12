/*******************************************************************************
 * 
 * File: DataLogger.cpp
 *
 * This file contains a generic interface for creating and managing Log
 * files on the robot.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#include "RobonautsLibrary/DataLogger.h"

#include "gsu/Advisory.h"
#include "RobonautsLibrary/RobotUtil.h"
#ifdef _2018
#include "DriverStation.h"
#else
#include "frc/DriverStation.h"
#endif

using namespace std;
using namespace frc;

/*******************************************************************************
 * 
 ******************************************************************************/
DataLogger::DataLogger(std::string path, std::string name, std::string type, uint32_t max_count, bool enabled)
    : m_segmented_file(path, name, type, max_count)
{
    m_file = NULL;
    m_enabled = enabled;
}

/*******************************************************************************
 *
 ******************************************************************************/
DataLogger::~DataLogger(void)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void DataLogger::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

/*******************************************************************************
 * 
 ******************************************************************************/
bool DataLogger::isEnabled(void)
{
	return m_enabled;
}

/*******************************************************************************
 *
 ******************************************************************************/
int32_t DataLogger::getCurrentIndex(void)
{
    return m_segmented_file.getCurrentIndex();
}

/*******************************************************************************
 *
 ******************************************************************************/
void DataLogger::openSegment(void)
{
	try
	{
		close();
		if (m_enabled)
		{
			DriverStation& ds = DriverStation::GetInstance();
			int match_type = ds.GetMatchType();
			if (match_type != DriverStation::kNone)
			{
				char aux_info[64];
				char type[] = {'n','p','q','e'};

				int match_number = ds.GetMatchNumber();
				sprintf(aux_info, "_%s_%c%03d", ds.GetEventName().c_str(), type[match_type], match_number);

				m_file = m_segmented_file.openNextSegment(aux_info);

				Advisory::pinfo("Opening log file %s", m_segmented_file.getSegmentName(
						m_segmented_file.getCurrentIndex(), std::string(aux_info)).c_str());
			}
			else
			{
				m_file = m_segmented_file.openNextSegment();

				Advisory::pinfo("Opening log file %s", m_segmented_file.getSegmentName(
						m_segmented_file.getCurrentIndex()).c_str());
			}
		}
	}
	catch (...)
	{
		m_file = NULL;
	}
}

/*******************************************************************************
 * 
 ******************************************************************************/
void DataLogger::log(const char* fmt, ...)
{
	try
	{
		if (m_file != NULL)
		{
			va_list args;
			va_start (args, fmt);
			vfprintf(m_file, fmt, args);
			va_end(args);
			fflush(m_file);
		}
	}
	catch(...)
	{
	}
}

/*******************************************************************************
 * 
 ******************************************************************************/
void DataLogger::flush()
{
	try
	{
		if (m_file != NULL)
		{
			fflush(m_file);
		}
	}
	catch (...)
	{	
	}
}

/*******************************************************************************
 * 
 ******************************************************************************/
void DataLogger::close()
{
	try
	{
		if (m_file != NULL)
		{
			fflush(m_file);
			fclose(m_file);
			m_file = NULL;
		}
	}
	catch (...)
	{
	}
}
