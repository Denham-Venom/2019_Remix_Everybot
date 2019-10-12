/*******************************************************************************
 *
 * File: MultiSpeedTrajectory.cpp
 *
 * This file contains a trajectory generator that can have different speeds
 * across the trajectory.  Trajectories are defined by distance of each segment
 * end velocity of each segment, and whether the initial trajectory matches the
 * prior segment's end velocity or the current segment's end velocity
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <string>
#include <vector>
//#include "gsu/Advisory.h"



/*******************************************************************************
 *
 ******************************************************************************/
class MultiSpeedTrajectory
{
public:
	MultiSpeedTrajectory(double T);
	bool Update(double time = -1);
	void ClearSegments();
	void AddSegment(double dist, double vel, bool blend);
	void ModifySegment(double dist, double vel, bool blend, int index);
	bool Initialize();
	void CalculateSegmentInitVelocity(std::vector<double> *end_vel, std::vector<bool> *blend, std::vector<double> *init_vel);
	double GetVel();
	double GetPos();
	double GetPercent();
	double GetEndDistance();
	void CalculatePathTimes(std::vector<double> *path_dist, std::vector<double> *init_vel, std::vector<double> *end_vel, std::vector<double> *path_time_cumm, std::vector<double> *path_time_incr);

	int GetSegment(double time);
	int GetCurrentSegment();
	double GetDistanceThroughSegment(int segment);
	double GetTrajectoryDuration();

private:
	double CalculateLength(std::vector<double> *seg_len, std::vector<double> *percent, std::vector<double> *incremental_length);
	double CalculateVelocity(double time, std::vector<double> *path_time_cumm, std::vector<double> *end_vel, std::vector<double> *init_vel, int segment);

	double CalculatePosition(double time, double vel, std::vector<double> *seg_dist, std::vector<double> *path_time_cumm, std::vector<double> *init_vel, int segment);
	double GetDeltaTime(double time, std::vector<double> *, int seg);
	double GetDeltaTimeForSegment(std::vector<double> *path_time_cumm, int segment);




    double m_T, m_time;
    bool m_running;
    double m_pos, m_vel;
    double m_percent;

    int m_current_segment;

    // input vectors
    std::vector<double> *m_path_distance;
    std::vector<bool> *m_blend;    // whether to blend velocity to point (or not (n-1 points))
    std::vector<double> *m_path_velocity_end;

    // internally calculated vectors
    std::vector<double> *m_percent_list;  // percent of distance along the path for each waypoint (first is zero, n points)
    std::vector<double> *m_path_velocity_init;  // velocity at start of segment
    std::vector<double> *m_path_time_cumm;
    std::vector<double> *m_path_time_incr;
    std::vector<double> *m_path_distance_incr;

    double m_path_length;

};

