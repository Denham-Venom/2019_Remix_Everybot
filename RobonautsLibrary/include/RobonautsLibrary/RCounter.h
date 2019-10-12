/*******************************************************************************
 *
 * File: RCounter.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#ifdef _2018
#include "Counter.h"
#else
#include "frc/Counter.h"
#endif

#include "RobonautsLibrary/RobotUtil.h"

#define SECONDS_TO_MINUTES 60.0

/*******************************************************************************
 *
 * This class provides a wrapper for the Counter class to convert counts into
 * RPM
 *  
 ******************************************************************************/
class RCounter : public Counter
{
	public:
#if defined (FRC_CRIO)
		RCounter(uint32_t slot, uint32_t channel, double filterCoeff = 0.9);
#else
		RCounter(uint32_t channel, double filterCoeff = 0.9);
#endif

		void Update();
		double GetSpeedRpm();
		double GetSpeedFiltered();
		double GetSpeedCountsPerSec();
		double GetSpeedRevsPerSec();
		double GetSpeedRpmFiltered();
		int32_t GetCounts();
		void SetCountPerRev(int cpr);
		void SetFilterCoeff(double coeff);
		void reset();

	private:
		int32_t counter_last_count;
		double counter_last_clock;
		double counter_counts_per_rev;

		double counter_speed_rpm_raw;
		double counter_speed_rps_raw;
		double counter_speed_cps_raw;
		double counter_speed_rpm_filter;
		double counter_low_pass_coeff;
		double counter_last_filter_data;
		bool counter_first;
};
