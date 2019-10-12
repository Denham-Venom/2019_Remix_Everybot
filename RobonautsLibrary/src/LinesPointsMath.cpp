 /*******************************************************************************
 *
 * File: LinesPointsMath.cpp
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
#include "RobonautsLibrary/LinesPointsMath.h"

#include <math.h>

// create default line through origin with zero slope
Line::Line()
{
    m_slope = 0.0;
    m_intercept = 0.0;
    m_is_vertical = false;
    m_angle_rad = 0.0;
    m_angle_deg = 0.0;
    m_p1.x = m_p1.y = m_p2.y = 0.0;
    m_p2.x = 1.0;
}

// copy constructor
Line::Line(Line &copy_line )
{
    copy_line.m_angle_deg = m_angle_deg;
    copy_line.m_angle_rad = m_angle_rad;
    copy_line.m_intercept = m_intercept;
    copy_line.m_is_vertical = m_is_vertical;
    copy_line.m_p1.x = m_p1.x;
    copy_line.m_p1.y = m_p1.y;
    copy_line.m_p2.x = m_p2.x;
    copy_line.m_p2.y = m_p2.y;
    copy_line.m_slope = m_slope;
}


// create a line from slope and a point, which could be the intercept
Line::Line(double slope, RobotUtil::Pnt *p)
{
    m_slope = slope;
    if (isnan(m_slope) == true)
    {
        m_intercept = p->x;
        m_is_vertical = true;
    }
    else
    {
        m_intercept = intercept(m_slope, p);
    }
}

// create a line from type two double x/y values)
Line::Line(double x1, double y1, double x2, double y2)
{
    createLine(x1, y1, x2, y2);
}

// create a line from two type Pnt
Line::Line(RobotUtil::Pnt *p1, RobotUtil::Pnt *p2)
{
    createLine(p1->x, p1->y, p2->x, p2->y);
}

// create line worker
void Line::createLine(double x1, double y1, double x2, double y2)
{
    // copy to points to be stored (possible for later use)
    m_p1.x = x1;
    m_p1.y = y1;
    m_p2.x = x2;
    m_p2.y = y2;

    if (x1 == x2)
    {
        m_slope = 0;   // actual slope is infinity
        m_intercept = x1;
        m_is_vertical = true;
    }
    else
    {
        m_slope = (y2 - y1) / (x2 - x1);
        m_intercept = intercept(m_slope, &m_p1);
    }
    m_angle_rad = std::atan2(y2 - y1, x2 - x1);
    m_angle_deg = m_angle_rad * RobotUtil::DEGREE_PER_RADIAN;
}

// calculate the intercept of a line
double Line::intercept(double slope, RobotUtil::Pnt *pt)
{
    return (pt->y - pt->x * slope); 
}

void Line::perpendicularLine(Line *line, RobotUtil::Pnt *p)
{
    double x1, y1, x2, y2;
    double perp_slope;

    if(m_is_vertical == true)
    {
        y1 = y2 = p->y;
        x1 = -1.0;
        x2 = 1.0;
        *line = Line(x1, y1, x2, y2);
    }
    else if (m_slope == 0)
    {
        x1 = x2 = p->x;
        y1 = -1.0;
        y2 = 1.0;
        *line = Line(x1, y1, x2, y2);
    }
    else
    {
        perp_slope = -1.0 / m_slope;
        *line = Line(perp_slope, p);
    }
}

double Line::getIntercept()
{
    return (m_intercept);
}

double Line::getSlope()
{
    return (m_slope);
}

double Line::getAngleRadians()
{
    return (m_angle_rad);
}

double Line::getAngleDegrees()
{
    return (m_angle_deg);
}

bool Line::isParallel(Line *l)
{
    // check for vertical slope two vertical slopes
    if(m_is_vertical == true && l->isVertical() == true)  // both vertical, parallel
    {
        return (true);
    }
	else if (m_is_vertical == true || l->isVertical() == true)  // one vertical (but not both), not parallel
	{
		return(false);
	}
    return (l->getSlope() == m_slope);
}

bool Line::isVertical()
{
    return (m_is_vertical);
}

bool Line::intersection(Line *l, RobotUtil::Pnt *intersect_pt)
{
    bool ret = true;
    if(isParallel(l) == true)
    {
        ret = false;
        intersect_pt->x = intersect_pt->y = 0.0;
    }
    else if (l->isVertical() == true)  // second line
    {
        intersect_pt->x = l->getIntercept();
        intersect_pt->y = intersect_pt->x * m_slope + m_intercept;
    }
    else if (m_is_vertical == true)
    {
        intersect_pt->x = m_intercept;
        intersect_pt->y = intersect_pt->x * l->getSlope() + l->getIntercept();
    }
    else
    {
        intersect_pt->x = (m_intercept - l->getIntercept()) / (l->getSlope() - m_slope);
        intersect_pt->y = m_slope * intersect_pt->x + m_intercept;
    }

    return (ret);
}

void Line::distanceAlongLineFromPoint(RobotUtil::Pnt pts_along_line[2], RobotUtil::Pnt *p, double dist)
{
    distanceAlongLineFromPoint(pts_along_line, p->x, p->y, dist);
}


void Line::distanceAlongLineFromPoint(RobotUtil::Pnt pts_along_line[2], double p_x, double p_y, double dist)
{
    double a, b, c;
    double sgn;
    //double x, y;
    //RobotUtil::Pnt pt;

    a = 1 + std::pow(m_slope, 2);
    b = 2 * (m_slope * (m_intercept - p_y) - p_x);
    c = std::pow(p_x, 2) + std::pow(m_intercept - p_y, 2) - std::pow(dist, 2);

    for (int i = 0; i < 2; i++)  // two solutions
    {
        sgn = std::pow(-1, i);
        if (m_is_vertical == false)
        {
            // quadratic equation
            pts_along_line[i].x = (-b + sgn * std::sqrt(std::pow(b, 2) - 4 * a * c)) / (2 * a);
            pts_along_line[i].y = m_slope * pts_along_line[i].x + m_intercept;
        }
        else   // vertical lines
        {
            pts_along_line[i].x = m_intercept;
            pts_along_line[i].y = sgn * dist + p_y;   // adding sign to get in right order for negative y vals
        }
    }

}

