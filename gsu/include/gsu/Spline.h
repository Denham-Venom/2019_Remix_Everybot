/*******************************************************************************
 *
 * File: Spline.h
 *
 ******************************************************************************/
#pragma once

#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>

namespace gsu
{

/*******************************************************************************
 *
 * Spline Interpolation
 *
 * This class is for doing a cubic spline interpolation given a set of known 
 * points. Note the points must be sorted such that the X coordinate is in 
 * ascending order.
 *
 * @Reference:
 *	This class is based on a code sample from
 *       http://kluge.in-chemnitz.de/opensource/spline/
 *
 * EXAMPLE USAGE:
 *
 * std::vector<double> time(9);
 * std::vector<double> x(9);
 * std::vector<double> y(9);
 *
 * time[0] = 0.0; time[1] = 10.0; time[2] = 20.0; time[3] = 30.0; time[4] = 40.0; time[5] = 50.0; time[6] = 60.0; time[7] = 70.0; time[8] = 80.0;
 * x[0] = 0.0;  x[1] = 5.0;   x[2] = 10.0;  x[3] = 10.0;  x[4] = 10.0;  x[5] = 5.0;  x[6] = 0.0;   x[7] = 0.0;  x[8] = 0.0;
 * y[0] = 0.0;  y[1] = 0.0;   y[2] = 0.0;   y[3] = 5.0;   y[4] = 10.0;  y[5] = 10.0; y[6] = 10.0;  y[7] = 5.0;  y[8] = 0.0;
 *
 * gsu::Spline spline_x;
 * spline_x.setPoints(time, x);
 *
 * gsu::Spline spline_y;
 * spline_y.setPoints(time, y);
 *
 * for (double t = time[0]; t < time[8]; t += 0.5)
 * {
 *	  printf("%8.6f, %8.6f, %8.6f\n", t, spline_x(t), spline_y(t));
 * }
 *
 ******************************************************************************/
class Spline
{
	public:
		Spline(void);
		Spline(const std::vector<double>& x, const std::vector<double>& y);
		Spline(const Spline &rh);
		virtual ~Spline(void);

		void setCurve(const std::vector<double>& x, const std::vector<double>& y);

		double evaluate(double x) const;
		double operator() (double x) const;

	private:
		std::vector<double> m_x;
		std::vector<double> m_y;            

		std::vector<double> m_a;
		std::vector<double> m_b;
		std::vector<double> m_c;
};

}
