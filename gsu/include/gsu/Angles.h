/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#pragma once

#include <algorithm>
#include <math.h>
#include <stdio.h>

namespace gsu
{

	namespace angles
	{

		template<typename Typ>
		static inline Typ from_degrees(Typ degrees) {
			return degrees * (Typ)(M_PI / 180.0);   
		}

		template<typename Typ>
		static inline Typ to_degrees(Typ radians) {
			return radians * (Typ)(180.0 / M_PI);
		}
    
		template<typename Typ>
		static inline Typ normalize_angle_positive(Typ angle) {
			return (Typ)fmod(fmod(angle, (Typ)(2.0*M_PI)) + (Typ)(2.0*M_PI), (Typ)(2.0*M_PI));
		}

	
		template<typename Typ>
		static inline Typ normalize_angle(Typ angle) {
			Typ a = normalize_angle_positive(angle);
			if (a > (Typ)M_PI)
			a -= (Typ)(2.0 *M_PI);
			return a;
		}

    
		template<typename Typ>
		static inline Typ shortest_angular_distance(Typ from, Typ to) {
			Typ result = normalize_angle_positive(normalize_angle_positive(to) - 
													 normalize_angle_positive(from));
			// If the result > 180,
			// It's shorter the other way.
			if (result > M_PI) result = -((Typ)(2.0*M_PI) - result);
        
			return normalize_angle(result);
		}

		template<typename Typ>
		static inline Typ two_pi_complement(Typ angle) {
			//check input conditions
			Typ TwoPi = (Typ)(2.0*M_PI);
			if (angle > TwoPi || angle < -TwoPi) angle = fmod(angle, TwoPi);    
			if(angle < 0) return (TwoPi + angle);
			else if (angle > 0) return (-TwoPi + angle);

			return(TwoPi);
		}

		template<typename Typ>
		static bool find_min_max_delta(Typ from,
									   Typ left_limit,
									   Typ right_limit,
									   Typ &result_min_delta,
									   Typ &result_max_delta) {
			Typ delta[4];

			delta[0] = shortest_angular_distance(from,left_limit);
			delta[1] = shortest_angular_distance(from,right_limit);

			delta[2] = two_pi_complement(delta[0]);
			delta[3] = two_pi_complement(delta[1]);

			if(delta[0] == 0) {
				result_min_delta = delta[0];
				result_max_delta = std::max<Typ>(delta[1],delta[3]);
				return true;
			}

			if(delta[1] == 0) {
				result_max_delta = delta[1];
				result_min_delta = std::min<Typ>(delta[0],delta[2]);
				return true;
			}


			Typ delta_min = delta[0];
			Typ delta_min_2pi = delta[2];
			if(delta[2] < delta_min) {
				delta_min = delta[2];
				delta_min_2pi = delta[0];
			}

			Typ delta_max = delta[1];
			Typ delta_max_2pi = delta[3];
			if(delta[3] > delta_max) {
				delta_max = delta[3];
				delta_max_2pi = delta[1];
			}


			//    printf("%f %f %f %f\n",delta_min,delta_min_2pi,delta_max,delta_max_2pi);
			if((delta_min <= delta_max_2pi) || (delta_max >= delta_min_2pi)) {
				result_min_delta = delta_max_2pi;
				result_max_delta = delta_min_2pi;
				if(left_limit == -M_PI && right_limit == M_PI) return true;
				else return false;
			}
			result_min_delta = delta_min;
			result_max_delta = delta_max;
			return true;
		}


		template<typename Typ>
		static inline bool shortest_angular_distance_with_limits(Typ from, 
																 Typ to,
																 Typ left_limit,
																 Typ right_limit,
																 Typ &shortest_angle) {

			Typ min_delta = (Typ)(-2.0*M_PI);
			Typ max_delta = (Typ)(2.0*M_PI);
			Typ min_delta_to = (Typ)(-2.0*M_PI);
			Typ max_delta_to = (Typ)(2.0*M_PI);
			bool flag    = find_min_max_delta(from,left_limit,right_limit,min_delta,max_delta);
			Typ delta = shortest_angular_distance(from,to);
			Typ delta_mod_2pi  = two_pi_complement(delta);


			if(flag) {	//from position is within the limits
				if(delta >= min_delta && delta <= max_delta) {
					shortest_angle = delta;
					return true;
				}
				else if(delta_mod_2pi >= min_delta && delta_mod_2pi <= max_delta) {
					shortest_angle = delta_mod_2pi;
					return true;
				}
				else {	//to position is outside the limits
					find_min_max_delta(to,left_limit,right_limit,min_delta_to,max_delta_to);
					if(fabs(min_delta_to) < fabs(max_delta_to)) {
						shortest_angle = std::max<double>(delta,delta_mod_2pi);
					}
					else if(fabs(min_delta_to) > fabs(max_delta_to)) {
						shortest_angle =  std::min<double>(delta,delta_mod_2pi);
					}
					else {
						if (fabs(delta) < fabs(delta_mod_2pi)) {
							shortest_angle = delta;
						}
						else shortest_angle = delta_mod_2pi;
					}
					return false;
				}
			}
			else {	 // from position is outside the limits
				find_min_max_delta(to,left_limit,right_limit,min_delta_to,max_delta_to);

				if(fabs(min_delta) < fabs(max_delta)) {
					shortest_angle = std::min<Typ>(delta,delta_mod_2pi);
				}
				else if (fabs(min_delta) > fabs(max_delta)) {
					shortest_angle =  std::max<Typ>(delta,delta_mod_2pi);
				}
				else {
					if (fabs(delta) < fabs(delta_mod_2pi)) {
						shortest_angle = delta;
					}
					else shortest_angle = delta_mod_2pi;
				}
				return false;
			}

			shortest_angle = delta;
			return false;
		}

	}	// namespace angles

} // namespace gsu

