/*******************************************************************************
 *
 * File: Time.cpp
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Time.h"

#include <chrono>

#include <cstring>
#include <cmath>

namespace gsi
{

/*******************************************************************************
 *
 * This method will return a time in seconds since some unspecified reference 
 * point.  The exact reference is system dependent and left unspecified to
 * help reduce overhead associated with calling this method.
 *
 * @return the time in seconds.
 *
 ******************************************************************************/
double Time::getTime(void)
{
	return (std::chrono::duration_cast< std::chrono::microseconds >(
		std::chrono::steady_clock::now().time_since_epoch())).count() / 1000000.0;
}

/*******************************************************************************
 *
 * This method will return a time in seconds since the system epoch 
 *  -- for UNIX, midnight of January 1, 1970, UTC.
 *
 * @return the time in seconds.
 *
 ******************************************************************************/
double Time::getTimeSinceEpoch(void)
{
	return (std::chrono::duration_cast< std::chrono::microseconds >(
		std::chrono::system_clock::now().time_since_epoch())).count() / 1000000.0;
}

/*******************************************************************************
 *
 * This method will fill the provided TimeStruct with the representation of
 * the current local time.
 *
 * @param time - a reference to a time structure that will be updated
 *
 ******************************************************************************/
void Time::getLocalTime(TimeStruct &time)
{
	auto now = std::chrono::system_clock::now();
	time_t t = std::chrono::system_clock::to_time_t(now);

	// NOTE: localtime and gmtime are not thread safe, they return a pointer to
	//       global memory that may be reused by other thread so copy local to
	//       help reduce (not prevent) the problem
	struct tm local;
	memcpy(&local, localtime(&t), sizeof(struct tm));
	struct tm utc;
	memcpy(&utc, gmtime(&t), sizeof(struct tm));

	int16_t offset = utc.tm_hour - local.tm_hour;
	if (offset < 0) offset += 24;

	std::chrono::microseconds ms = std::chrono::duration_cast< std::chrono::microseconds >(
		now.time_since_epoch());

	double intg;
	double frac = modf(ms.count() / 1000000.0, &intg);

	time.year             = local.tm_year + 1900;
	time.day_of_year      = local.tm_yday; 
	time.month            = local.tm_mon + 1;  
	time.day_of_month     = local.tm_mday; 
	time.hour             = local.tm_hour; 
	time.minute           = local.tm_min;  
	time.second           = local.tm_sec;  
	time.day_of_week      = local.tm_wday + 1; 
	time.utc_offset_hours = (uint8_t)offset;
	time.dst_offset_hours = local.tm_isdst;
	time.microsecond = (uint32_t)(frac * 1000000.0);
}

/*******************************************************************************
 *
 * This method will fill the provided TimeStruct with the representation of
 * the current UTC time.
 *
 * @param time - a reference to a time structure that will be updated
 *
 ******************************************************************************/
void Time::getUtcTime(TimeStruct &time)
{
	auto now = std::chrono::system_clock::now();
	time_t t = std::chrono::system_clock::to_time_t(now);

	// NOTE: gmtime is not thread safe, it return a pointer to
	//       global memory that may be reused by other thread so copy local to
	//       help reduce (not prevent) the problem
	struct tm utc;
	memcpy(&utc, gmtime(&t), sizeof(struct tm));

	std::chrono::microseconds ms = std::chrono::duration_cast< std::chrono::microseconds >(
		now.time_since_epoch());

	double intg;
	double frac = modf(ms.count() / 1000000.0, &intg);

	time.year             = utc.tm_year + 1900;
	time.day_of_year      = utc.tm_yday; 
	time.month            = utc.tm_mon + 1;  
	time.day_of_month     = utc.tm_mday; 
	time.hour             = utc.tm_hour; 
	time.minute           = utc.tm_min;  
	time.second           = utc.tm_sec;  
	time.day_of_week      = utc.tm_wday + 1; 
	time.utc_offset_hours = 0;
	time.dst_offset_hours = 0;
	time.microsecond      = (uint32_t)(frac * 1000000.0);
}

/*******************************************************************************
 *
 * Get the current local time formatted as a string.  Several formatting options
 * are available, if a formatting option is not specified a default of
 * YYYY.MM.DD HH:MM:SS will be used.
 *
 * @param	fmt		a format enumeration that indicates the format of the string
 *
 * @return a string representation of the local time
 *
 ******************************************************************************/
std::string Time::getTimeString(TimeStringFormat fmt)
{
	TimeStruct time;
	getLocalTime(time);
	return getTimeString(time, fmt);
}

/*******************************************************************************
 *
 * Convert the provided TimeStruct into a string with a common format.
 * 
 * @param	time	A time structure that should be converted to a string
 * @param	fmt		a format enumeration that indicates the format of the string
 *
 * @return a string representation of the local time
 *
 ******************************************************************************/
std::string Time::getTimeString(TimeStruct &time, TimeStringFormat fmt)
{
	char time_str[64];

	switch (fmt)
	{
		case Time::FORMAT_YMD_:
			sprintf(time_str,"%04d_%02d_%02d", time.year,
				time.month, time.day_of_month);
			break; 

		case FORMAT_HMS_: 
			sprintf(time_str,"%02d%02d%02d", time.hour, 
				time.minute, time.second);
			break;

		case FORMAT_YMDHMS_:
			sprintf(time_str, "%04d_%02d_%02d_%02d%02d%02d", time.year,
				time.month, time.day_of_month, time.hour,
				time.minute, time.second);
			break;

		case FORMAT_YMDHMSu_:
			sprintf(time_str, "%04d_%02d_%02d_%02d%02d%02d_%06d", time.year,
				time.month, time.day_of_month, time.hour,
				time.minute, time.second, time.microsecond);
			break;

		case FORMAT_YMD: 
			sprintf(time_str, "%04d.%02d.%02d", time.year,
				time.month, time.day_of_month);
			break; 

		case FORMAT_HMS: 
			sprintf(time_str, "%02d:%02d:%02d", time.hour,
				time.minute, time.second);
			break;

		case FORMAT_YMDHMSu:
			sprintf(time_str, "%04d.%02d.%02d %02d:%02d:%02d.%06d", time.year,
				time.month, time.day_of_month, time.hour,
				time.minute, time.second, time.microsecond);
			break;

		default: 
			sprintf(time_str, "%04d.%02d.%02d %02d:%02d:%02d", time.year,
				time.month, time.day_of_month, time.hour,
				time.minute, time.second);
			break;
	}

	return(std::string(time_str));
}

} // namespace gsi
