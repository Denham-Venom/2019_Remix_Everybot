/*******************************************************************************
 *
 * File: Time.h
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdint.h>
#include <string>

#define MILLISECONDS_PER_SECOND 1000
#define MICROSECONDS_PER_SECOND 1000000

#define SECONDS_PER_MINUTE 		60
#define MILLISECONDS_PER_MINUTE	60000
#define MICROSECONDS_PER_MINUTE 60000000

#define MINUTES_PER_HOUR 		60
#define SECONDS_PER_HOUR 		3600
#define MILLISECONDS_PER_HOUR	3600000
#define MICROSECONDS_PER_HOUR 	3600000000

#define SECONDS_TO_MILLISEC(s) ((uint64_t))((s*1000))
#define SECONDS_TO_MICROSEC(s) ((uint64_t))((s*1000000))
#define MICROSECS_TO_SECONDS(us) (us/1000000.0)
#define MILLISECS_TO_SECONDS(ms) (ms/1000.0)

namespace gsi
{

/*******************************************************************************
 *
 * This structure can hold a representation of the time with the fields split
 * into common elements.
 *
 * NOTE: This structure is 16 bytes long, the fields are arranged to fit on
 *       word boundaries so there should never be any padding introduced by
 *       a compiler. The contents of the fields that are longer than one byte
 *       are normally in host byte order.
 *
 ******************************************************************************/
struct TimeStruct
{
	uint32_t microsecond;      // microsecond of second 0 to 999999
	int16_t  year;             // year since 0000
	uint16_t day_of_year;      // day of year 0 = January 1
	uint8_t  month;            // month 1 to 12
	uint8_t  day_of_month;     // day of month 1 to 31
	uint8_t  hour;             // hour of day 0 to 23
	uint8_t  minute;           // minute of hour 0 to 59
	uint8_t  second;           // second of minute 0 to 59
	uint8_t  day_of_week;      // 1 to 7
	uint8_t  utc_offset_hours; // 0 to 23
	uint8_t  dst_offset_hours; // 0 or 1
};

/*******************************************************************************
 *
 * This class provides an object oriented way to interact with Time and clocks.
 *
 ******************************************************************************/
class Time
{
	public:
		// format indicators that end with an underscore should be 
		// safe for file and/or folder names
		enum TimeStringFormat
		{
			FORMAT_YMDHMS,  FORMAT_YMD,  FORMAT_HMS, FORMAT_YMDHMSu,
			FORMAT_YMDHMS_, FORMAT_YMD_, FORMAT_HMS_, FORMAT_YMDHMSu_
		};

		static double getTime(void);
		static double getTimeSinceEpoch(void);

		static void getLocalTime(TimeStruct &time);
		static void getUtcTime(TimeStruct &time);

		// @TODO: add methods:
		//     double localToUtc(double utc);
		//     double utcToLocal(double local);
		//     TimeStruct to double, see mktime()
		//     double to TimeStruct (local and UTC), see gmtime() and localtime()

		static std::string getTimeString(TimeStringFormat fmt = FORMAT_YMDHMS);
		static std::string getTimeString(TimeStruct &time, TimeStringFormat fmt = FORMAT_YMDHMS);
};

} // namespace gsi
