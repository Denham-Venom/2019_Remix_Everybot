    //************************************************************************
    // Class to build and execute a cartesian trajectory, includes ability
    // to have variable velocities on individual segments
    //************************************************************************
#include "RobonautsLibrary/MultiSpeedTrajectory.h"
#include "RobonautsLibrary/LinesPointsMath.h"

class CartTrajectory
{
	public:
		CartTrajectory(double T);
		double GetX();
		double GetY();
		std::vector<double> *GetXList();
		std::vector<double> *GetYList();
		double GetEndDist();
		double GetCurrentDist();
		double GetCurrentVelocity();
		double GetHeading();
		double GetFinalHeading();
		double GetSegmentHeading();
		double GetTotalTime();
		void SetX(double x);
		void SetY(double y);
		void Update(double time=-1.0);
		//void AddPoint(double x, double y, double vel, bool blend);
		void AddPoint(double x, double y, double vel, bool blend, int num_round_pts=0, double round_dist = 0.0);
		void ModifyPoint(double x, double y, double vel, bool blend, int index);
		void ClearTrajectory(bool reset_to_zero=true, double x=0.0, double y=0.0);
		void Initialize();
		double GetPercent(double current, double end, double start);
		bool IsRunning();

	private:
		double m_x, m_y;

		double m_heading;
		double m_end_heading;

		bool m_running;
		double m_T;   // period in seconds, default is 50 hz

		MultiSpeedTrajectory *m_traj;

		// List of cartesian points (n points)
		std::vector<double> *m_x_list;
		std::vector<double> *m_y_list;
		std::vector<double> *m_x_list_round;
		std::vector<double> *m_y_list_round;
		std::vector<double> *m_vel_list;
		std::vector<bool> *m_blend_list;
		std::vector<double> *m_round_dist;
		std::vector<int> *m_num_round_pts;
		std::vector<double> *m_segment_headings;

		double CalculateFinalHeading(std::vector<double> *x, std::vector<double> *y);
		void CalculateSegmentHeadings();
		double  CalculateHeading(double x, double x_prev, double y, double y_prev);
		void CalculateXY(double distance, unsigned int segment);

		// functions supporting rounding of corners
		void AddPointSegment(double x_new, double y_new, double x_prev, double y_prev, double vel, bool blend);

		void replan();   // coordinates replanning activities
		double getRoundDistance(double des_round_distance, int idx);  // calculates how much to round a point (uses member variables)
		void getCircleOrigin(RobotUtil::Pnt origins[4], double x, double y, double round_dist, Line lines[2]);
		int getBestCircle(Line line[2], RobotUtil::Pnt origins[4], double x_i_1, double y_i_1, double x_i, double y_i);
		void getRoundStartPoints(Line line[2], RobotUtil::Pnt round_start_end[2], int idx, double x, double y, double round_dist);
		double planRound(RobotUtil::Pnt round_start_end[2], RobotUtil::Pnt *center, int round_pts, double *start_angle, double *radius);
        
};
