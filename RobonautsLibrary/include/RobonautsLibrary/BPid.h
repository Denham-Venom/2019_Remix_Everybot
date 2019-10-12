/*******************************************************************************
 *
 * File: BPid.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#define HIGHPASS	0
#define	GAIN_SCHEDULE 0

const double DEFAULT_INTEGRATOR_THRESH = 150.0; // high number due to significant quantization

const double DEFAULT_HIGH_PASS_COEFF = 0.8f;

/*******************************************************************************
 *
 * This class is used for closed loop control of high speed motors that 
 * will see rapid changes in loading
 * 
 ******************************************************************************/
class BPid
{
	public:
		BPid();
		~BPid();

		void setGains(double kp, double ki, double kd, double threshold=100000000.0f);
		void setFilterCoefficient(double filter_coeff) {m_filter_coeff = filter_coeff; };
		void setLimits(double lower, double upper);
		void setFeedFowardCoefficient(double a0, double a1, double a2, double a3);
		void setIsAngle(bool is_angle);

		void disable();
		void enable();
		void setState(bool enabledA);
		void reset();
		void reset_internal();

		double getP() { return m_kp;};
		double getI() { return m_ki;};
		double getD() { return m_kd;};
		double getDeltaErrorFiltered() { return m_delta_err_filter; };
		double getDeltaErrorRaw() { return m_delta_err; };
		double getError() { return m_error; };
		double getErrorSum() { return m_prev_error; };

		double update(double reference, double actual);

	private:
		double m_kp; // proportional gain
		double m_ki; // integral gain
		double m_kd; // integral gain
		double m_ref; // setpoint desired

		double m_ffwd_0; // feed forward coefficient (input^3)
		double m_ffwd_1; // feed forward coefficient (input^2)
		double m_ffwd_2; // feed forward coefficient (input^1)
		double m_ffwd_3; // feed forward coefficient (input^0)

		double m_actual; // actual or measured value
		double m_mot_cmd; // output of controller, usually a motor command [-1,1]
		double m_prev_error; // sum of all prior error, subject to decay
		double m_delta_err;  // difference between current and prior m_error (can be low passed)
		double m_delta_err_filter;  // difference between current and prior m_error (can be low passed)
		double m_filter_coeff;
		double m_error; // kept for logging
		double m_limits[2]; // limits on output of controller [0] = lower, [1] = upper

		// threshold for turning on integrator part of controller
		double m_integrate_threshold;

		bool m_is_angle;  // if the PID is an angle, how error is treated differently (error between -179 and 179 is 2, not -358)

#if HIGHPASS	
		// highpass filter (to be used later)
		double last_highpass_output;
		double last_highpass_input;
		double hp_coeff;
#endif

		bool m_enabled;
		bool m_gains_set;
		bool m_limits_set;

		double feedForward(double ref);
};

