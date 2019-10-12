/*******************************************************************************
 *
 * File: Polynomial.h
 *
 ******************************************************************************/
#include <stdint.h>
#include <vector>
#include <cassert>

namespace gsu 
{

/*******************************************************************************
 *
 ******************************************************************************/
class Polynomial
{
	public:
		Polynomial(void);
		Polynomial(uint8_t order);
		Polynomial(std::vector<double> coefficients);
		Polynomial(const Polynomial &rh);
		virtual ~Polynomial(void);

		void setOrder(uint8_t order);
		uint8_t getOrder(void);

		void setCoefficients(std::vector<double> coefficients);
		void setCoefficient(uint8_t c, double value);
		double getCoefficient(uint8_t c);

		bool setCurve(std::vector<double> x, std::vector<double> y); // do a best fit of these points
		bool setCurve(double x[], double y[], int num_points);  // do a best fit of these points

		double evaluate(double x) const;
		double operator() (double x) const;

	private:
		uint8_t m_order;
		std::vector<double> m_coefficients;
};

}
