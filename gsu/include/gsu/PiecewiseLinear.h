/***************************************************************************//**
 *
 ******************************************************************************/
#pragma once

#include <stdint.h>
#include <vector>

namespace gsu 
{

/***************************************************************************//**
 *
 ******************************************************************************/
class PiecewiseLinear
{
	public:
		PiecewiseLinear(void);
		PiecewiseLinear(double x[], double y[], uint16_t points);
		PiecewiseLinear(std::vector<double> x, std::vector<double> y);
		PiecewiseLinear(const PiecewiseLinear &rh);
		~PiecewiseLinear(void);

		void setCurve(double x[], double y[], uint16_t points);
		void setCurve(std::vector<double> x, std::vector<double> y);

		double evaluate(double x) const;
		double operator() (double x) const;

	private:
		double evaluate(double x, double x0, double y0, double x1, double y1) const;

		uint16_t m_points;
		std::vector<double> m_points_x;
		std::vector<double> m_points_y;
};

}
