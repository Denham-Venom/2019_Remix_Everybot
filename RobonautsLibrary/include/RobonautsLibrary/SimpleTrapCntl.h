/*******************************************************************************
 *
 * File: SimpleTrapCntl.h - Trapizoidal Position Control
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <string>

/*******************************************************************************
 *
 * This Trapezoid Velocity Control of Position for doing closed loop control
 *
 * This control is intended to adjust the output to a new position using
 * a velocity curve that forms a trapezoid -- accelerate, hold at max, then
 * decelerate. This helps get to the position without overshooting it.
 *
 * Note the Kf, Kp, Ki, and Kd terms are for an internal FPID loop that is
 * controlling the velocity not the position -- so how do you tune this?
 *
 * Start with the feed forward term (Kf), it should the control value
 * needed to get the control surface to move at max velocity. In my
 * case I want the output (arm joint) to rotate at 120 degrees / second. To do
 * this the motor needs to be commanded to about 0.6 -- so Kf = 0.6.
 *
 * Next, pick a Kp and set Ki and Kd to zero. An initial Kp can be selected
 * by Kp = Kf / 50.0. Then do some testing and collect some data increase Kp
 * until you see some oscillation in the actual velocity and maybe the control
 * surface, then decrease Kp until that oscillation just goes away.
 *
 * Now, if you are still not happy with the position response, start with a
 * Ki = Kp / 100.0, then adjust that to get closer to the target velocity
 *
 * Finally, you can ad a Kd to make it more responsive, Kd should start with
 * a value about the same as Ki, as adjusting Kd the other values (mostly Ki)
 * may also need to be adjusted.
 *
 ******************************************************************************/
class SimpleTrapCntl
{
	public:
		SimpleTrapCntl();
		~SimpleTrapCntl(void);

		void setControlConstants(float f, float p, float i, float d);
		void setVelocityLimits(float min, float max, float thp = 0.20);
		void setAccelerationLimits(float val);
		void setControlValueLimits(float min, float max);

		void reset(void);

		float calculateControlValue(float targ_val, float feedback_val);
		void resetControlValue(float targ_val, float feedback_val);

		float getErrorP(void);
		float getErrorI(void);
		float getErrorD(void);
		
		float getTargetVel(void);
		float getTargetPos(void);
		float getMeasuredVel(void);
		
		float getVelocityMin(void);
		float getVelocityMax(void);
		float getControlMin(void);
		float getControlMax(void);
		float getThreshold(void);
		
		float limitTarget(float val);
		float limitControl(float val);

		void updateConfig(void);
		void setParamPrefix(std::string prefix);
		
	private:
		float stc_const_f; // feed forward control factor
		float stc_const_p; // proportional control factor
		float stc_const_i; // integral control factor
		float stc_const_d; // differential control factor

		float stc_error_p; // last p error term
		float stc_error_i; // last i error term
		float stc_error_d; // last d error term

		float stc_vel_max; // variables for target (or set point) and the
		float stc_vel_min; // feedback (or process) values

		float stc_cntl_max; // variables for control (or manipulated) values
		float stc_cntl_min;

		float stc_accel;
		
		float stc_threshold; // percentage of velocity range, large changes in the target will cause
		float stc_previous_target_vel; // the internal accumulators to be reset
		float stc_previous_feedback_vel;
		
		double stc_previous_time;
		float stc_previous_feedback;
		float stc_previous_decel_needed;
		
		float stc_target_pos;
		float stc_tolerance;
		float stc_error_i_degrade;

		std::string stc_param_prefix;  // refix for all parameters for this PID
};
