 /*******************************************************************************
 *
 * File: LinesPointsMath.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 * Classes and structures that allow basic line and point geometry
 * 
 ******************************************************************************/
#pragma once

#include "RobonautsLibrary/RobotUtil.h"
#include <math.h>

using namespace std;

class Line
{
    public:
        Line();
        Line(Line &copy_line);
        Line(double slope, RobotUtil::Pnt *p);
        Line(double x1, double y1, double x2, double y2);
        Line(RobotUtil::Pnt *p1, RobotUtil::Pnt *p2);

        void perpendicularLine(Line *line, RobotUtil::Pnt *p);

        double getIntercept();
        double getSlope();
        double getAngleRadians();
        double getAngleDegrees();
        bool isParallel(Line *l);
        bool isVertical();
        bool intersection(Line *l, RobotUtil::Pnt *intersect_pt);
        void distanceAlongLineFromPoint(RobotUtil::Pnt pts_along_line[2], double x, double y, double dist);
        void distanceAlongLineFromPoint(RobotUtil::Pnt pts_along_line[2], RobotUtil::Pnt *p, double dist);


    private:
        void createLine(double x1, double y1, double x2, double y2);
        double intercept(double slp, RobotUtil::Pnt *pt);

        double m_slope = 0.0, m_intercept = 0.0;
        bool m_is_vertical = false;
        RobotUtil::Pnt m_p1, m_p2;

        double m_angle_rad = 0, m_angle_deg = 0;
    

};

