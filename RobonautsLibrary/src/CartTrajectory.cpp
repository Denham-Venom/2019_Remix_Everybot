/*******************************************************************************
 *
 * File: CartTrajectory.cpp
 *
 * Class to build and execute a cartesian trajectory, includes ability
 * to have variable velocities on individual segments
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#include <math.h>
#include <cmath>
#include "RobonautsLibrary/CartTrajectory.h"
#include <string>

//#include "gsu/Advisory.h"

using namespace std;


//**********************************************************************************************
// constructor, takes period, in seconds for the loop to use for calculating trajectories
//**********************************************************************************************
CartTrajectory::CartTrajectory(double T)
{
	m_x = 0.0;
	m_y = 0.0;
	m_heading = m_end_heading = 0.0;
	m_running = false;

	m_x_list = new std::vector<double>();
	m_y_list = new std::vector<double>();
	m_segment_headings = new std::vector<double>();

	m_x_list_round = new std::vector<double>();
	m_y_list_round = new std::vector<double>();
	m_vel_list = new std::vector<double>();
	m_blend_list = new std::vector<bool>();
	m_round_dist = new std::vector<double>();
	m_num_round_pts = new std::vector<int>();

	m_T = T;

	//double m_T = 0.02;   // period in seconds, default is 50 hz

	m_traj = new MultiSpeedTrajectory(m_T);

	ClearTrajectory();
}

//**********************************************************************************************
// return the current X setpoint
//**********************************************************************************************
double CartTrajectory::GetX()
{
	return (m_x);
}

//**********************************************************************************************
// return the current Y setpoint
//**********************************************************************************************
double CartTrajectory::GetY()
{
	return (m_y);
}

//**********************************************************************************************
// return the current X setpoint list
//**********************************************************************************************
std::vector<double> * CartTrajectory::GetXList()
{
	return (m_x_list_round);
}

//**********************************************************************************************
// return the current Y setpoint list
//**********************************************************************************************
std::vector<double>* CartTrajectory::GetYList()
{
	return (m_y_list_round);
}

//**********************************************************************************************
// return the end distance
//**********************************************************************************************
double CartTrajectory::GetEndDist()
{
	return (m_traj->GetEndDistance());
}

//**********************************************************************************************
// return the current distance
//**********************************************************************************************
double CartTrajectory::GetCurrentDist()
{
	return (m_traj->GetPos());
}

//**********************************************************************************************
// return the current distance
//**********************************************************************************************
double CartTrajectory::GetTotalTime()
{
	return (m_traj->GetTrajectoryDuration());
}


//**********************************************************************************************
// return the current Velocity
//**********************************************************************************************
double CartTrajectory::GetCurrentVelocity()
{
	return (m_traj->GetVel());
}


//**********************************************************************************************
// return the current heading setpoint (likely not used for differential drive robots)
//**********************************************************************************************
double CartTrajectory::GetHeading()
{
	return (m_heading);
}

//**********************************************************************************************
// return the final heading of the trajectory
//**********************************************************************************************
double CartTrajectory::GetFinalHeading()
{
	return (m_end_heading);
}

//**********************************************************************************************
// return the heading of the current segment
//**********************************************************************************************
double CartTrajectory::GetSegmentHeading()
{
	unsigned int segment = m_traj->GetCurrentSegment();
	if (segment >= m_segment_headings->size())
	{
		segment = m_segment_headings->size() - 1;
	}
	return (m_segment_headings->at(segment));
}

//**********************************************************************************************
// initialize the X cmd
//**********************************************************************************************
void CartTrajectory::SetX(double x)
{
	m_x = x;
}

//**********************************************************************************************
// initialize the Y cmd
//**********************************************************************************************
void CartTrajectory::SetY(double y)
{
	m_y = y;
}

//**********************************************************************************************
// periodic update, first updates the distance along the path, then calculates the XY
// coordinate along the path
//**********************************************************************************************
void CartTrajectory::Update(double time)
{
	m_traj->Update(time);

	CalculateXY(m_traj->GetPos(), m_traj->GetCurrentSegment());

}

//**********************************************************************************************
// Add a waypoint to the path
//**********************************************************************************************
void CartTrajectory::AddPoint(double x, double y, double vel, bool blend, int num_round_pts, double round_dist)
{
	double dx, dy;
	int idx;
	m_x_list->push_back(x);
	m_y_list->push_back(y);

	idx = m_x_list->size() - 1;   // index of most recent x, y
	dx = m_x_list->at(idx) - m_x_list->at(idx - 1);
	dy = m_y_list->at(idx) - m_y_list->at(idx - 1);
	m_round_dist->push_back(round_dist);
	m_num_round_pts->push_back(num_round_pts);
	m_blend_list->push_back(blend);
	m_vel_list->push_back(vel);

	m_traj->AddSegment(std::sqrt(dx * dx + dy * dy), vel, blend);
}

void CartTrajectory::AddPointSegment(double x_new, double y_new, double x_prev, double y_prev, double vel, bool blend)
{
	double dx, dy;

	m_x_list_round->push_back(x_new);
	m_y_list_round->push_back(y_new);

	dx = x_new - x_prev;
	dy = y_new - y_prev;

	m_traj->AddSegment(std::sqrt(dx * dx + dy * dy), vel, blend);
}


//**********************************************************************************************
// modify a waypoint by index
//**********************************************************************************************
void CartTrajectory::ModifyPoint(double x, double y, double vel, bool blend, int index)
{
	double dx, dy;
	m_x_list_round->at(index) = x;
	m_y_list_round->at(index) = y;

	try   // will fail when moving first waypoint
	{
		dx = m_x_list_round->at(index) - m_x_list_round->at(index-1);
		dy = m_y_list_round->at(index) - m_y_list_round->at(index-1);
		m_traj->ModifySegment(std::sqrt(dx * dx + dy * dy), vel, blend, index);
	}
	catch(...)
	{

	}

	try   // will fail when moving last waypoint
	{
		dx = m_x_list_round->at(index+1) - m_x_list_round->at(index);
		dy = m_y_list_round->at(index+1) - m_y_list_round->at(index);
		m_traj->ModifySegment(std::sqrt(dx * dx + dy * dy), vel, blend, index + 1);
	}
	catch(...)
	{

	}
}

//**********************************************************************************************
// Clear all the existing waypoints, re-initialize first waypoint to [0, 0], perhaps look for do non-zero in future
//**********************************************************************************************
void CartTrajectory::ClearTrajectory(bool reset_to_zero, double x, double y)
{
	m_segment_headings->clear();
	m_y_list->clear();
	m_x_list->clear();
	m_y_list_round->clear();
	m_x_list_round->clear();
	m_round_dist->clear();
	m_num_round_pts->clear();
	m_blend_list->clear();
	m_vel_list->clear();

	if(reset_to_zero == true)
	{
		m_x_list->push_back(0.0);
		m_y_list->push_back(0.0);
	}
	else
	{
		m_x_list->push_back(x);
		m_y_list->push_back(y);
	}
	m_vel_list->push_back(0.0);
	m_blend_list->push_back(true);

	m_traj->ClearSegments();
}

//**********************************************************************************************
// initialize the trajectory
//**********************************************************************************************
void CartTrajectory::Initialize()
{
	replan();
	m_end_heading = CalculateFinalHeading(m_x_list_round, m_y_list_round);
	CalculateSegmentHeadings();
	m_traj->Initialize();
	m_running = true;
}

//**********************************************************************************************
// BEGIN support of rounding cartesian corners
//**********************************************************************************************

//**********************************************************************************************
// function that coordinates rounding of corners
//**********************************************************************************************
void CartTrajectory::replan()
{
	Line two_lines_prox_distal[2];
	RobotUtil::Pnt circle_origins[4];
	RobotUtil::Pnt line_start_end[2];
	double start_angle=0, delta_angle, angle;
	double radius=0;
	double round_dist;
	double x, y;
	int idx=-1;
	double x_i, y_i;
	int end_idx;
	int round_end_idx;

	m_x_list_round->clear();
	m_y_list_round->clear();
	m_segment_headings->clear();
	m_traj->ClearSegments();


	m_x_list_round->push_back(m_x_list->at(0));
	m_y_list_round->push_back(m_y_list->at(0));

	for (unsigned int i=1;i<m_x_list->size()-1;i++)
	{
		// shorthand for later
		x_i = m_x_list->at(i);
		y_i = m_y_list->at(i);

		// get equation of lines formed ahead of and after current point in traj
		for(unsigned int j=0;j<2; j++)
		{
			two_lines_prox_distal[j] = Line(m_x_list->at(i - 1 + j), m_y_list->at(i - 1 + j), m_x_list->at(i + j), m_y_list->at(i + j));
		}
		// don't round parallel line or points that don't want rounding (which is zero or one points)
		if (two_lines_prox_distal[0].isParallel(&(two_lines_prox_distal[1])) == false && m_num_round_pts->at(i) > 1)
		{
			// get length of rounded section
			round_dist = getRoundDistance(m_round_dist->at(i), i);

			// get origins of circles formed by four solutions
			getCircleOrigin(circle_origins, x_i, y_i, round_dist, two_lines_prox_distal);

			// overly complex way to determine which is best of four solutions
			// idx[0]=prox1,dist1, idx[1]=prox1,dist2, idx[2]=prox2,dist1, idx[3]=prox2,dist2
			idx = getBestCircle(two_lines_prox_distal, circle_origins, m_x_list->at(i - 1), m_y_list->at(i - 1), x_i, y_i);

			// get length down the proximal and distal lines to form circles for tangent lines
			// idx[0]=prox1,dist1, idx[1]=prox1,dist2, idx[2]=prox2,dist1, idx[3]=prox2,dist2
			getRoundStartPoints(two_lines_prox_distal, line_start_end, idx, x_i, y_i, round_dist);

			// plan the rounds and add them to the list
			delta_angle = planRound(line_start_end, &(circle_origins[idx]), m_num_round_pts->at(i), &start_angle, &radius);
			for(int ii=0;ii<m_num_round_pts->at(i);ii++)
			{
				angle = delta_angle * ii + start_angle;
				x = radius * std::cos(angle / RobotUtil::DEGREE_PER_RADIAN) + circle_origins[idx].x;
				y = radius * std::sin(angle / RobotUtil::DEGREE_PER_RADIAN) + circle_origins[idx].y;
				end_idx = m_x_list_round->size() - 1;
				AddPointSegment(x, y, m_x_list_round->at(end_idx), m_y_list_round->at(end_idx), m_vel_list->at(i), m_blend_list->at(i));
			}
		}
		else
		{
			end_idx = m_x_list_round->size() - 1;
			AddPointSegment(x_i, y_i, m_x_list_round->at(end_idx), m_y_list_round->at(end_idx), m_vel_list->at(i), m_blend_list->at(i));
		}

	}
	end_idx = m_x_list->size() - 1;
	round_end_idx = m_x_list_round->size()-1;
	AddPointSegment(m_x_list->at(end_idx), m_y_list->at(end_idx), m_x_list_round->at(round_end_idx), m_y_list_round->at(round_end_idx), m_vel_list->at(end_idx), m_blend_list->at(end_idx));

/*
	FILE *fp;
	fp = fopen("/robot/logs/drive/xy.csv", "w");
	if (fp != nullptr)
	{
		fprintf(fp, "x,y\n");
		for (unsigned int i = 0; i < m_x_list->size(); i++)
		{
			fprintf(fp,"%lf,%lf\n", m_x_list->at(i), m_y_list->at(i));
		}
		fclose(fp);
	}
*/
}

//**********************************************************************************************
// Get rounding distance; minimum of half the distance between the segments and stated value
//**********************************************************************************************
double CartTrajectory::getRoundDistance(double des_round_distance, int idx)
{
	double len[2];

	for (int i = 0; i < 2; i++)
	{
		len[i] = RobotUtil::CartesianDistance(m_x_list->at(idx - 1 + i), m_y_list->at(idx - 1 + i), m_x_list->at(idx + i), m_y_list->at(idx + i));
	}

	// make a function
	return(std::min(std::min(len[0]/2.0, len[1]/2.0), des_round_distance));
}

//**********************************************************************************************
// While planning rounding, get the origin of the four options to round intersecting lines
//**********************************************************************************************
void CartTrajectory::getCircleOrigin(RobotUtil::Pnt origins[4], double x, double y, double round_dist, Line lines[2])
{
	// get length down the proximal and distal lines to form circles for tangent lines
	// idx[0]=prox1,dist1, idx[1]=prox1,dist2, idx[2]=prox2,dist1, idx[3]=prox2,dist2
	RobotUtil::Pnt dist_down_line[4];  // don't use head so it doesn't create on the stack
	Line perp_line[4]; 
	int i;

	for (i=0;i<2;i++)
	{
		lines[i].distanceAlongLineFromPoint(&(dist_down_line[i*2]), x, y, round_dist);
	}

	// get perpendicular lines on each                    
	for (i = 0; i < 4; i++)
	{
		lines[i/2].perpendicularLine(&(perp_line[i]), &(dist_down_line[i]));
	}

	int cnt=0;
	for (int j = 0; j < 2; j++)
	{
		for (int k = 0; k < 2; k++)
		{
			 perp_line[j].intersection(&(perp_line[k + 2]), &(origins[cnt]));
			 cnt++;
		}
	}
}

//**********************************************************************************************
// Select the correct circle by transforming circles into coordinate system of two lines
//**********************************************************************************************
int CartTrajectory::getBestCircle(Line line[2], RobotUtil::Pnt origins[4], double x_i_1, double y_i_1, double x_i, double y_i)
{
	RobotUtil::Transform2D T;
	RobotUtil::Transform2D Tinv;
	int idx1 = -1, idx2 = -1;
	bool arc_to_left = false;
	RobotUtil::Pnt xformed_pts[4];
	int idx=-1;

	// determine concavity of three points/two line segments
	if (RobotUtil::getMinAngleError(line[0].getAngleDegrees(), line[1].getAngleDegrees()) < 0)
	{
		arc_to_left = true;
	}

	// create coordinate transformation from axis (with line formed by previous and current point as x) and origin
	RobotUtil::TransformationMatrix2D(&T, line[0].getAngleRadians(), x_i_1, y_i_1);
	RobotUtil::InvertTransformationMatrix2D(&Tinv, &T);

	for (int i = 0; i < 4; i++)
	{
		// transform circle origin to line formed with prior ponts coordinate system 
		RobotUtil::TransformationMult(&(xformed_pts[i]), &Tinv, &(origins[i]));

		// if robot arcs left and circle is +y (or arcs right and is -y) in new coordinate system, save candidate for further eval
		if (((xformed_pts[i].y > 0 && arc_to_left == true) ||
			(xformed_pts[i].y < 0 && arc_to_left == false)) && idx1 == -1)
		{
			idx1 = i;
		}
		else if ((xformed_pts[i].y > 0 && arc_to_left == true) || (xformed_pts[i].y < 0 && arc_to_left == false))
		{
			idx2 = i;
			break;
		}
	}
	// create coordinate transformation from axis (with line formed by current point and next point as x) and origin
	RobotUtil::TransformationMatrix2D(&T, line[1].getAngleRadians(), x_i, y_i);
	RobotUtil::InvertTransformationMatrix2D(&Tinv, &T);
	RobotUtil::TransformationMult(&(xformed_pts[0]), &Tinv, &(origins[idx1]));
	RobotUtil::TransformationMult(&(xformed_pts[1]), &Tinv, &(origins[idx2]));

	idx = idx2;
	// get answer; if robot arcs left and circle is +y (or arcs right and is -y) in new coordinate system, it's the answer
	if ((xformed_pts[0].y > 0 && arc_to_left == true) || (xformed_pts[0].y < 0 && arc_to_left == false))
	{
		idx = idx1;
	}

	// idx[0]=prox1,dist1, idx[1]=prox1,dist2, idx[2]=prox2,dist1, idx[3]=prox2,dist2
	return(idx);
}

//**********************************************************************************************
// get the intersection of the circles at the tangets to the lines around the corner that's being rounded 
//**********************************************************************************************
void CartTrajectory::getRoundStartPoints(Line line[2], RobotUtil::Pnt round_start_end[2], int idx, double x, double y, double round_dist)
{
	RobotUtil::Pnt down_line[4];

	// get length down the proximal and distal lines to form circles for tangent lines
	// idx[0]=prox1,dist1, idx[1]=prox1,dist2, idx[2]=prox2,dist1, idx[3]=prox2,dist2
	for (int i = 0; i < 2; i++)
	{
		line[i].distanceAlongLineFromPoint(&(down_line[i*2]),x, y, round_dist);
	}

	switch (idx)
	{
		case 0:
			round_start_end[0] = down_line[0];
			round_start_end[1] = down_line[2];
			break;
		case 1:
			round_start_end[0] = down_line[0];
			round_start_end[1] = down_line[3];
			break;
		case 2:
			round_start_end[0] = down_line[1];
			round_start_end[1] = down_line[2];
			break;
		case 3:
			round_start_end[0] = down_line[1];
			round_start_end[1] = down_line[3];
			break;
		default:
			break;
	}
}

//**********************************************************************************************
// calculate the x/y points to round the the corner
//**********************************************************************************************
double CartTrajectory::planRound(RobotUtil::Pnt round_start_end[2], RobotUtil::Pnt *center, int round_pts, double *start_angle, double *radius)
{
	double dx, dy, end_angle;
	dx = round_start_end[0].x - center->x;
	dy = round_start_end[0].y - center->y;
	*start_angle = std::atan2(dy, dx) * RobotUtil::DEGREE_PER_RADIAN;
	end_angle = std::atan2(round_start_end[1].y - center->y, round_start_end[1].x - center->x) * RobotUtil::DEGREE_PER_RADIAN;
	*radius = std::sqrt(dx * dx + dy * dy);
	return(-1.0 * (RobotUtil::getMinAngleError(*start_angle, end_angle) / (round_pts - 1.0)));

}

//**********************************************************************************************
// END support of rounding cartesian corners
//**********************************************************************************************

//**********************************************************************************************
// Get the percent along the path of the current point
//**********************************************************************************************
double CartTrajectory::GetPercent(double current, double end, double start)
{
	return (current/(end-start));
}

//**********************************************************************************************
// return true of the trajectory is running
//**********************************************************************************************
bool CartTrajectory::IsRunning()
{
	return (m_running);
}

//**********************************************************************************************
//**********************************************************************************************
double CartTrajectory::CalculateFinalHeading(std::vector<double> *x, std::vector<double> *y)
{
	double heading = 0.0;
	int x_size = x->size(), y_size = y->size();
	if (x_size > 1)  // one length lists can't have a delta
	{
		heading = CalculateHeading(x->at(x_size - 1), x->at(x_size - 2), y->at(y_size - 1), y->at(y_size - 2));
	}
	return (heading);
}

//**********************************************************************************************
// Calculate heading for each segments; works off member variables instead of arguments
//**********************************************************************************************
void CartTrajectory::CalculateSegmentHeadings()
{
	double heading;
	m_segment_headings->clear();
	for (unsigned int i = 1;i<m_x_list_round->size();i++)    // starting index at 1 intentionally
	{
		heading = CalculateHeading(m_x_list_round->at(i), m_x_list_round->at(i-1), m_y_list_round->at(i), m_y_list_round->at(i-1));
		m_segment_headings->push_back(heading);
	}
}

//**********************************************************************************************
// Calculate heading for each segments; works off member variables instead of arguments
//**********************************************************************************************
double CartTrajectory::CalculateHeading(double x, double x_prev, double y, double y_prev)
{
	double dx, dy;

	dx = x - x_prev;
	dy = y - y_prev;
	return(RobotUtil::DEGREE_PER_RADIAN * std::atan2(dy, dx));
}

//**********************************************************************************************
// Get XY from percent along trajectory
//**********************************************************************************************
void CartTrajectory::CalculateXY(double distance, unsigned int segment)
{
	double distance_to_prev_segment = m_traj->GetDistanceThroughSegment(segment);
	double remaining_dist = distance - distance_to_prev_segment;  // distance along current segment
	double x_to_prev = 0.0, y_to_prev = 0.0;
	double angle_rad;

	if (segment >= m_segment_headings->size())
	{
		m_x = m_x_list_round->at(m_x_list_round->size() - 1);
		m_y = m_y_list_round->at(m_y_list_round->size() - 1);
		m_running = false;
	}
	else
	{
		x_to_prev = m_x_list_round->at(segment);
		y_to_prev = m_y_list_round->at(segment);
		angle_rad = m_segment_headings->at(segment) / RobotUtil::DEGREE_PER_RADIAN;
		m_x = x_to_prev + std::cos(angle_rad) * remaining_dist;
		m_y = y_to_prev + std::sin(angle_rad) * remaining_dist;
	}
}

