/*******************************************************************************
 *
 * File: SimplePID.h
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
 * This Simple PID id for doing closed loop control
 * 
 ******************************************************************************/
class SimplePID
{
	public:
		SimplePID();
		~SimplePID(void);

		void setPIDConstants(float p, float i, float d);
		void setTargetLimits(float min, float max, float thp = 0.20);
		void setControlLimits(float min, float max);

		void reset(void);

		float calculateControlValue(float targ_val, float feedback_val);

		float getErrorP(void);
		float getErrorI(void);
		float getErrorD(void);
		float getKp(void);
		float getKi(void);
		float getKd(void);
		
		
		float getTargetMin(void);
		float getTargetMax(void);
		float getControlMin(void);
		float getControlMax(void);
		float getThreshold(void);
		
		float limitTarget(float val);
		float limitControl(float val);

		void updateConfig(void);
		void setParamPrefix(std::string prefix);
		
	private:
//		float limit(float val, float min, float max);

		float stc_const_p; // proportional control factor
		float spid_const_i; // integral control factor
		float spid_const_d; // differential control factor

		float spid_error_p; // last p error term
		float spid_error_i; // last i error term
		float spid_error_d; // last d error term

		float spid_targ_max; // variables for target (or set point) and the
		float spid_targ_min; // feedback (or process) values

		float spid_cntl_max; // variables for control (or manipulated) values
		float spid_cntl_min;

		float spid_threshold; // percentage of range, large changes in the target will cause
		float spid_prev_targ; // the internal accumulators to be reset
		
		std::string spid_param_prefix;  // refix for all parameters for this PID
};
