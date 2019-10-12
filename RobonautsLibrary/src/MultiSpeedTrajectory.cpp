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

#include <math.h>
#include "RobonautsLibrary/MultiSpeedTrajectory.h"
#include <string>

using namespace std;


//*************************************************************************
// A just beyond basic Trapezoidal profile generator
// Initial and final velocity are zero
// Can have asymmetric accel and decel profile
// runs a sim of the trajectory then adjusts the position to handle
//     quantization and round off error
//*************************************************************************
MultiSpeedTrajectory::MultiSpeedTrajectory(double T)
{
    m_T = T;
    m_time = 0.0;
    m_running = false;
    m_pos = m_vel = 0.0;
    m_percent = 0.0;

    m_current_segment = 0;
    m_path_length = 0.0;

    // input vectors
    m_path_distance = new std::vector<double>();
    m_blend = new std::vector<bool>();    // whether to blend velocity to point (or not (n-1 points))
    m_path_velocity_end = new std::vector<double>();

    // internally calculated vectors
    m_percent_list = new std::vector<double>();  // percent of distance along the path for each waypoint (first is zero, n points)
    m_path_velocity_init = new std::vector<double>();  // velocity at start of segment
    m_path_time_cumm = new std::vector<double>();
    m_path_time_incr = new std::vector<double>();
    m_path_distance_incr = new std::vector<double>();
}

//*******************************************************************************************
// Periodic update of trajectory state.  Either works on internally kept time (if time arg
// is < 0) or externally kept time (if time is >0)
//*******************************************************************************************
bool MultiSpeedTrajectory::Update(double time)
{
	if (time >= 0)
	{
		m_time = time;
	}
	m_current_segment = GetSegment(m_time);
	m_vel = CalculateVelocity(m_time, m_path_time_cumm, m_path_velocity_end, m_path_velocity_init, m_current_segment);
	m_pos = CalculatePosition(m_time, m_vel, m_path_distance, m_path_time_cumm, m_path_velocity_init, m_current_segment);
	try
	{
		m_percent = m_pos / m_path_length;
	}
	catch(...)
	{
		m_percent = 1.0;
	}
	if (m_pos >= GetEndDistance())
	{
		m_running = false;  // trajectory is complete
	}
	if (time < 0)
	{
		m_time += m_T;
	}

	return (m_running);
}

//*******************************************************************************************
// re-initialize all the lists used within the class, then add the first point back
//*******************************************************************************************
void MultiSpeedTrajectory::ClearSegments()
{
	m_percent_list->clear();
	m_path_time_cumm->clear();
	m_path_time_incr->clear();
	m_path_velocity_end->clear();
	m_path_velocity_init->clear();
	m_path_distance->clear();
	m_path_distance_incr->clear();
	m_blend->clear();

	m_percent_list->push_back(0.0);
}

//*******************************************************************************************
// add a new segment to the list
//*******************************************************************************************
void MultiSpeedTrajectory::AddSegment(double dist, double vel, bool blend)
{
	m_path_distance->push_back(dist);
	m_path_velocity_end->push_back(vel);
	m_blend->push_back(blend);
}

//*******************************************************************************************
// modify a segment by index
//*******************************************************************************************
void MultiSpeedTrajectory::ModifySegment(double dist, double vel, bool blend, int index)
{
	m_path_distance->at(index - 1) = dist;
	//            m_path_velocity_end[index] = vel;
	//            m_blend[index] = blend;
}

//*******************************************************************************************
// initialize a trajectory before execution.  Determine total path length, calculate
// initial velocity of segment based on state of blend varialbe, then calculate times
// for each segment based, both incremental and cummulative
//*******************************************************************************************
bool MultiSpeedTrajectory::Initialize()
{
	m_running = true;

	m_time = 0.0;

	m_pos = m_vel = 0.0;  // initialize (will probably change in future to having non-zero values)

	m_path_length = CalculateLength(m_path_distance, m_percent_list, m_path_distance_incr);

	CalculateSegmentInitVelocity(m_path_velocity_end, m_blend, m_path_velocity_init);
	CalculatePathTimes(m_path_distance, m_path_velocity_init, m_path_velocity_end, m_path_time_cumm, m_path_time_incr);

	return (m_running);
}

//*******************************************************************************************
// calculate the initial velocity of each segment.  If blend = true, it is end velocity of
// prior segment, if blend = false, it is end velocity of current segment
//*******************************************************************************************
void MultiSpeedTrajectory::CalculateSegmentInitVelocity(std::vector<double> *end_vel, std::vector<bool> *blend, std::vector<double> *init_vel)
{
	if (end_vel->size() > 0)  // no zero length lists
	{
		init_vel->clear();
		for (unsigned int i = 0; i < end_vel->size(); i++)
		{
			if (i == 0)  // initial velocity is zero by default
			{
				init_vel->push_back(0);
			}
			else
			{
				if (blend->at(i) == true)
				{
					init_vel->push_back(end_vel->at(i - 1));
				}
				else
				{
					init_vel->push_back(end_vel->at(i));
				}
			}
		}
	}
}


//*******************************************************************************************
// return current velocity of the segment
//*******************************************************************************************
double MultiSpeedTrajectory::GetVel()
{
	return (m_vel);
}

//*******************************************************************************************
// return current position of the segment
//*******************************************************************************************
double MultiSpeedTrajectory::GetPos()
{
	return (m_pos);
}

//*******************************************************************************************
// return percent along the trajectory
//*******************************************************************************************
double MultiSpeedTrajectory::GetPercent()
{
	return (m_percent);
}

//*******************************************************************************************
// return total distance of the trajectory
//*******************************************************************************************
double MultiSpeedTrajectory::GetEndDistance()
{
	return (GetDistanceThroughSegment(m_path_distance->size()));
}

//*******************************************************************************************
// return total distance of the trajectory
//*******************************************************************************************
double MultiSpeedTrajectory::GetTrajectoryDuration()
{
	return (this->m_path_time_cumm->at(m_path_time_cumm->size()-1));
}

//*******************************************************************************************
// calculate the incremental and cummulative times for each segment of trajectory
//*******************************************************************************************
void MultiSpeedTrajectory::CalculatePathTimes(std::vector<double> *path_dist, std::vector<double> *init_vel, std::vector<double> *end_vel, std::vector<double> *path_time_cumm, std::vector<double> *path_time_incr)
{
	double cumm_time = 0.0;
	// see next gen curves excel file for info on this

	path_time_cumm->clear();
	m_path_time_incr->clear();

	for (unsigned int i = 0; i < path_dist->size(); i++)
	{
		try
		{
			m_path_time_incr->push_back(2.0 * path_dist->at(i) / (init_vel->at(i) + end_vel->at(i)));
			cumm_time += path_time_incr->at(i);
		}
		catch(...)  // likely caused by error in config file where init and end vel are both 0, give a little init_vel to at least be functional
		{
			// TODO: fix this by checking the trajectory as it comes in!
			m_path_time_incr->push_back(2.0 * path_dist->at(i) / (init_vel->at(i) + end_vel->at(i) + 5.0));
			cumm_time += path_time_incr->at(i);
			cumm_time += 0.0;
		}
		path_time_cumm->push_back(cumm_time);
	}

}

//*******************************************************************************************
// calculate the total distance of a trajectory and the percent at each segment
//*******************************************************************************************
double MultiSpeedTrajectory::CalculateLength(std::vector<double> *seg_len, std::vector<double> *percent, std::vector<double> *incremental_length)
{
	double total_dist = 0.0;
	unsigned int i;

	for (i = 0; i < seg_len->size(); i++)               {
		total_dist += seg_len->at(i);
	}
	percent->clear();
	for (i = 0; i < seg_len->size(); i++)   // starting at 1 intentionally
	{
		if (i == 0)
		{
			percent->push_back(seg_len->at(i) / total_dist);
		}
		else if (total_dist > 0.0)
		{
			percent->push_back(seg_len->at(i) / total_dist + percent->at(i - 1));
		}
		else  // for case where initial and only point is [0,0]
		{
			percent->push_back(1.0);
		}
	}
	return (total_dist);
}

//*******************************************************************************************
// get the segment along a trajectory based on time
//*******************************************************************************************
int MultiSpeedTrajectory::GetSegment(double time)
{
	unsigned int i;
	for(i= 0;i<m_path_time_cumm->size();i++)
	{
		if(time < m_path_time_cumm->at(i))
		{
			break;
		}
	}
	return (i);
}

//*******************************************************************************************
// get the segment the trajectory is currently working on
//*******************************************************************************************
int MultiSpeedTrajectory::GetCurrentSegment()
{
	return (m_current_segment);
}

//*******************************************************************************************
// calculate the cummulative distance through a segment number
//*******************************************************************************************
double MultiSpeedTrajectory::GetDistanceThroughSegment(int segment)
{
	double dist = 0.0;
	for(int i=0;i<segment;i++)
	{
		dist += m_path_distance->at(i);
	}
	return (dist);
}

//*******************************************************************************************
// calculate the velocity based on time and the current segment
//*******************************************************************************************
double MultiSpeedTrajectory::CalculateVelocity(double time, std::vector<double> *path_time_cumm, std::vector<double> *end_vel, std::vector<double> *init_vel, int segment)
{
	double vel = 0.0;
	double delta_time;
	double delta_time_seg;
	double slope;
	int num_segments = path_time_cumm->size();

	if (segment < num_segments)    // velocity is zero after all segments
	{
		delta_time = GetDeltaTime(time, path_time_cumm, segment);
		delta_time_seg = GetDeltaTimeForSegment(path_time_cumm, segment);  // should this simply be m_path_time_incr[segment]??
		try
		{
			slope = (end_vel->at(segment) - init_vel->at(segment)) / delta_time_seg;
			vel = slope * delta_time + init_vel->at(segment);
		}
		catch(...)
		{
			vel = 0.0;
		}
	}
	return (vel);
}

//*******************************************************************************************
// calculate current position of trajectory based on time
//*******************************************************************************************
double MultiSpeedTrajectory::CalculatePosition(double time, double vel, std::vector<double> *seg_dist, std::vector<double> *path_time_cumm, std::vector<double> *init_vel, int segment)
{
	double dist = 0.0;
	double delta_time;
	int num_segments = path_time_cumm->size();

	for (int i = 0; i < segment; i++)
	{
		dist += seg_dist->at(i);
	}
	if (segment < num_segments)
	{
		delta_time = GetDeltaTime(time, path_time_cumm, segment);
		dist += 0.5 * delta_time * (init_vel->at(segment) + vel);
	}
	return (dist);
}

//*******************************************************************************************
// calcuate and return time between start of current segment and given segment
//*******************************************************************************************
double MultiSpeedTrajectory::GetDeltaTime(double time, std::vector<double> *path_time_cumm, int seg)
{
	double dt = 0;
	if (seg == 0)
	{
		dt = time;
	}
	else
	{
		dt = (time - path_time_cumm->at(seg - 1));
	}
	return (dt);
}

//*******************************************************************************************
// calcuate and return the incremental time between two segments (isn't this kept as a list??
//*******************************************************************************************
double MultiSpeedTrajectory::GetDeltaTimeForSegment(std::vector<double> *path_time_cumm, int segment)
{
	double dt = 0;
	if(segment == 0)
	{
		dt = path_time_cumm->at(0);
	}
	else
	{
		dt = m_path_time_cumm->at(segment) - path_time_cumm->at(segment - 1);
	}
	return (dt);
}

