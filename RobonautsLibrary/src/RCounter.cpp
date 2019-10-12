/*******************************************************************************
 *
 * File: RCounter.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#ifdef _2018
#include "Timer.h"
#else
#include "frc/Timer.h"
#endif

#include "RobonautsLibrary/RCounter.h"

using namespace frc;

#if defined (FRC_CRIO)
RCounter::RCounter(uint32_t slot, uint32_t channel, double filterCoeff) :
	Counter(slot, channel)
#else
RCounter::RCounter(uint32_t channel, double filterCoeff) :
	Counter(channel)
#endif
{
	reset();
	counter_counts_per_rev = 1; // initialize

	counter_low_pass_coeff = filterCoeff;
}

/******************************************************************************
 *	reset timing
 ******************************************************************************/
void RCounter::reset()
{
//	counter_last_clock = GetClock();
	counter_last_clock = Timer::GetFPGATimestamp();
	counter_last_count = 0;

	counter_speed_rpm_raw = 0.0;
	counter_speed_rpm_filter = 0.0;
	counter_first = true;
	counter_last_filter_data = 0.0;

	Reset(); // reset counter
}

/******************************************************************************
 *	udpate counter
 ******************************************************************************/
void RCounter::Update()
{
	double curr_clock = Timer::GetFPGATimestamp();
//	double curr_clock = GetClock();
	uint32_t counts = this->Get(); // read number of counts

	if (curr_clock != counter_last_clock) // don't process if it will result in divide by zero
	{
		counter_speed_cps_raw = (counts - counter_last_count) / (curr_clock
		    - counter_last_clock);
		counter_speed_rps_raw = counter_speed_cps_raw / counter_counts_per_rev;
		counter_speed_rpm_raw = counter_speed_rps_raw * SECONDS_TO_MINUTES;

		counter_speed_rpm_filter = RobotUtil::lowPass(counter_speed_rpm_raw,
		    &counter_last_filter_data, counter_low_pass_coeff);
	}
	else
	{
		counter_speed_cps_raw = 0.0;
		counter_speed_rps_raw = 0.0;
		counter_speed_rpm_raw = 0.0;

		counter_speed_rpm_filter = RobotUtil::lowPass(counter_speed_rpm_raw,
		    &counter_last_filter_data, counter_low_pass_coeff);
	}

	counter_last_clock = curr_clock; // save time for next time through
	counter_last_count = counts; // save counts for next time through
}

/******************************************************************************
 *
 ******************************************************************************/
void RCounter::SetFilterCoeff(double coeff)
{
	counter_low_pass_coeff = coeff;
}

/******************************************************************************
 *	return speed in RPM (raw)
 ******************************************************************************/
double RCounter::GetSpeedRpm()
{
	return (counter_speed_rpm_raw);
}

/******************************************************************************
 *	return speed in RPM (raw)
 ******************************************************************************/
double RCounter::GetSpeedRpmFiltered()
{
	return (counter_speed_rpm_filter);
}

/******************************************************************************
 *	set number of counts/rev
 ******************************************************************************/
void RCounter::SetCountPerRev(int cpr)
{
	if (cpr < 1) // can't have less than 1 cpr
	{
		cpr = 1;
	}
	counter_counts_per_rev = cpr;
}

/******************************************************************************
 *	get raw counts
 ******************************************************************************/
int32_t RCounter::GetCounts()
{
	return (Get());
}

/******************************************************************************
 *	get speed in counts per second
 ******************************************************************************/
double RCounter::GetSpeedCountsPerSec()
{
	return (counter_speed_cps_raw);
}

/******************************************************************************
 *	get speed in revs per second
 ******************************************************************************/
double RCounter::GetSpeedRevsPerSec()
{
	return (counter_speed_rps_raw);
}
