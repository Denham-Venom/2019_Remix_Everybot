/***************************************************************************//**
 *
 * @File PiecewiseLinear.cpp
 *
 ******************************************************************************/
#include "gsu/PiecewiseLinear.h"
#include <cmath>

namespace gsu
{

/***************************************************************************//**
 *
 ******************************************************************************/
PiecewiseLinear::PiecewiseLinear(void)
{
	setCurve(std::vector<double>(), std::vector<double>());
}

/***************************************************************************//**
 *
 * Create an instance of a PiecewiseLinear class that can be used to
 * evaluate data points.
 *
 * The user must make sure there are at least 2 points
 * The user must make sure points are in order from smallest x to largest x
 *
 * @param points	- the number of points that define the curve
 * @param	x		- an array of the x values of the points
 * @param	y		- na array of the y values of the points
 *
 ******************************************************************************/
	PiecewiseLinear::PiecewiseLinear(double x[], double y[], uint16_t points)
{
	setCurve(x, y, points);
}

/***************************************************************************//**
 *
 ******************************************************************************/
PiecewiseLinear::PiecewiseLinear(std::vector<double> x, std::vector<double> y)
{
	setCurve(x, y);
}

/***************************************************************************//**
 *
 * A copy constructor that will allow for a deep copy of a PieceWiseLinear
 * object.
 *
 * @param rh	- a reference to the right hand side of the assignment (the 
 *				  object to copy).
 *
 ******************************************************************************/
PiecewiseLinear::PiecewiseLinear(const PiecewiseLinear &rh)
{
	m_points = rh.m_points;
	m_points_x = rh.m_points_x;
	m_points_y = rh.m_points_y;
}

/***************************************************************************//**
 *
 * Release the arrays used to hold the points on the curve.
 *
 ******************************************************************************/
PiecewiseLinear::~PiecewiseLinear(void)
{
	m_points_x.clear();
	m_points_y.clear();
	m_points = 0;
}

/***************************************************************************//**
 *
 ******************************************************************************/
void PiecewiseLinear::setCurve(double x[], double y[], uint16_t points)
{
	setCurve(std::vector<double>(x, x + points), std::vector<double>(y, y + points));
}

/***************************************************************************//**
 *
 ******************************************************************************/
void PiecewiseLinear::setCurve(std::vector<double> x, std::vector<double> y)
{
	m_points = x.size();
	if (m_points == 0)
	{
		double temp_x[] = { 0.0, 1.0 };
		double temp_y[] = { 0.0, 0.0 };

		m_points = 2;
		m_points_x = std::vector<double>(temp_x, temp_x + 2);
		m_points_y = std::vector<double>(temp_y, temp_y + 2);
	}
	else if (m_points == 1)
	{
		double temp_x[] = { 0.0, 1.0 };
		double temp_y[] = { y[0], y[0] };

		m_points = 2;
		m_points_x = std::vector<double>(temp_x, temp_x + 2);
		m_points_y = std::vector<double>(temp_y, temp_y + 2);
	}
	else
	{
		m_points_x = x;
		m_points_y = y;
	}
}

/***************************************************************************//**
 *
 * Evaluate a single point on the curve.
 *
 * @param x	- the value to evalueate
 *
 * @return the y value for the given x
 *
 ******************************************************************************/
double PiecewiseLinear::evaluate(double x) const
{
	for (int i = 1; i < m_points; i++)
	{
		if (x < m_points_x[i])
		{
			return evaluate(x, m_points_x[i - 1], m_points_y[i - 1], m_points_x[i], m_points_y[i]);
		}
	}

	return evaluate(x, m_points_x[m_points - 2], m_points_y[m_points - 2], m_points_x[m_points - 1], m_points_y[m_points - 1]);
}

/***************************************************************************//**
 *
 * This private method is used to evaluate a point on a given segment of the
 * curve.
 *
 * @param x		- the value to evaluate
 * @param x0	- the x value of the first point that defines the segment
 * @param y0	- the y value of the first point that defines the segment
 * @param x1	- the x value of the second point that defines the segment
 * @param y1	- the y value of the second point that defines the segment
 *
 * @return the y value for the provided x
 *
 ******************************************************************************/
inline double PiecewiseLinear::evaluate(double x, double x0, double y0, double x1, double y1) const
{
	// y = (m * x) + b
	// m = (y1 - y0) / (x1 - x0)
	// b = y0 - (m * x0)
	// y = (m * x) + y0 - (m * x0)
	//   = (m * (x - x0)) + y0
	//   = (((y1 - y0) / (x1 - x0)) * (x - x0)) + y0;
	return (((y1 - y0) / (x1 - x0)) * (x - x0)) + y0;
}

}

