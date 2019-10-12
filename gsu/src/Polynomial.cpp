/*******************************************************************************
 *
 * File: Polynomial.cpp
 *
 ******************************************************************************/
#include "gsu/Polynomial.h"
#include <cmath>

namespace gsu
{
/***************************************************************************//**
 *
 ******************************************************************************/
Polynomial::Polynomial(void)
{
	setOrder(0);
}

/***************************************************************************//**
 *
 ******************************************************************************/
Polynomial::Polynomial(uint8_t order)
{
	setOrder(order);
}

/***************************************************************************//**
 *
 ******************************************************************************/
Polynomial::Polynomial(std::vector<double> coefficients)
{
	setCoefficients(coefficients);
}

/***************************************************************************//**
 *
 ******************************************************************************/
Polynomial::Polynomial(const Polynomial &rh)
{
	m_order = rh.m_order;
	m_coefficients = rh.m_coefficients;
}

/***************************************************************************//**
 *
 ******************************************************************************/
Polynomial::~Polynomial(void)
{
	m_order = 0;
	m_coefficients.clear();
}

/***************************************************************************//**
 *
 ******************************************************************************/
void Polynomial::setOrder(uint8_t order)
{
	m_order = order;
	m_coefficients.resize(m_order + 1);
	for (int i = 0; i <= m_order; i++)
	{
		m_coefficients[i] = 0.0;
	}
}

/***************************************************************************//**
 *
 ******************************************************************************/
uint8_t Polynomial::getOrder(void)
{
	return m_order;
}

/***************************************************************************//**
 *
 ******************************************************************************/
void Polynomial::setCoefficients(std::vector<double> coefficients)
{
	if (coefficients.size() > 0)
	{
		m_order = coefficients.size() - 1;
		m_coefficients = coefficients;
	}
	else
	{
		m_order = 0;
		m_coefficients.resize(1);
		m_coefficients[0] = 0.0;
	}
}

/***************************************************************************//**
 *
 ******************************************************************************/
void Polynomial::setCoefficient(uint8_t c, double value)
{
	if (c <= m_order)
	{
		m_coefficients[c] = value;
	}
}

/***************************************************************************//**
 *
 ******************************************************************************/
double Polynomial::getCoefficient(uint8_t c)
{
	if (c <= m_order)
	{
		return m_coefficients[c];
	}

	return 0.0;
}

/***************************************************************************//**
 *
 ******************************************************************************/
double Polynomial::evaluate(double x) const
{
	double y = m_coefficients[0];
	double x_pow = x;
	for (uint8_t i = 1; i <= m_order; ++i)
	{
		y += m_coefficients[i] * x_pow;
		x_pow *= x;
	}
	return y;
}

/***************************************************************************//**
 *
 ******************************************************************************/
double Polynomial::operator() (double x) const
{
	return evaluate(x);
}

/***************************************************************************//**
 *
 * param x - an array of the x coordinate of the data set
 * param y - an array of the y coordinate of the data set
 * param num_points - the number of points in the data set
 * 
 * true if the coeff array was updated to the new coefficients
 *
 ******************************************************************************/
bool Polynomial::setCurve(double x[], double y[], int num_points)
{
	return setCurve(std::vector<double>(x, x + num_points), std::vector<double>(y, y + num_points));
}

/***************************************************************************//**
 *
 * param x - a vector of the x coordinate of the data set
 * param y - a vector of the y coordinate of the data set
 * 
 * true if the coeff array was updated to the new coefficients
 *
 ******************************************************************************/
bool Polynomial::setCurve(std::vector<double> x, std::vector<double> y)
{
	uint32_t num_points = x.size();
	assert(num_points > 1);
	assert(num_points = y.size());

	int equation_count = m_order + 1;

	// Store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
	double* sigmas = new double[2 * m_order + 1];
	for (int i = 0; i < 2 * m_order + 1; ++i)
	{
		sigmas[i] = 0;
		for (uint32_t j = 0; j < num_points; ++j)
		{
			sigmas[i] += pow(x[j], i);
		}
	}

	int B_width = equation_count + 1;
	int B_height = equation_count;
	double* B = new double[B_width * B_height];
	for (int i = 0; i < equation_count; ++i)
	{
		for (int j = 0; j < equation_count; ++j)
		{
			B[i * B_width + j] = sigmas[i + j];
		}
	}
	delete[] sigmas;

	// Store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi) in the final column of B.
	for (int i = 0; i < m_order + 1; ++i)
	{
		B[i * B_width + equation_count] = 0;
		for (uint32_t j = 0; j < num_points; ++j)
		{
			B[i * B_width + equation_count] += pow(x[j], i) * y[j];
		}
	}

	// Perform gaussian elimination, i.e. turn the matrix into one in row echelon form.
	// TODO move the row with the largest absolute value value to the pivot positioning.
	//      This improves numerical stability, but is otherwise not needed.
	for (int i = 0; i < equation_count - 1; ++i)
	{
		for (int k = i + 1; k < equation_count; ++k)
		{
			double t = B[k * B_width + i] / B[i * B_width + i];
			for (int j = 0; j <= equation_count; ++j)
			{
				B[k * B_width + j] = B[k * B_width + j] - t*B[i * B_width + j];
			}
		}
	}

	// Back-substitution.
	for (int i = equation_count - 1; i >= 0; --i) 
	{
		double a = B[i * B_width + equation_count];
		for (int j = i + 1; j < equation_count; ++j)
		{
			a -= B[i * B_width + j] * m_coefficients[j];
		}
		m_coefficients[i] = float(a / B[i * B_width + i]);
	}

	delete[] B;

	return true;
}

}

