/*----------------------------------------------------------------------------*/
/* Robonauts abstraction of IR analog sensor                                  */
/* Initially used during 2018 for line sensors                                */
/*----------------------------------------------------------------------------*/

#pragma once

#ifdef _2018
#include "AnalogInput.h"
#else
#include "frc/AnalogInput.h"
#endif
using namespace frc;

#define DEFAULT_LATCH_CYLES 4
#define DEFAULT_THRESHOLD_VALUE 3900
#define DEFAULT_IR_VALUE 4100
/**
 * 
 */
class RAnalogIrSensor : public AnalogInput
{
 public:

  enum ThresholdType { kGreaterThan, kLessThan };

  RAnalogIrSensor(int channel);
  ~RAnalogIrSensor();

  void setThreshold(int threshold);
  void setLatchCycles(int cycles);
  void setThresholdType(ThresholdType type);
  bool onLine();
  int update();

 private:
  int m_threshold;
  ThresholdType m_threshold_type;
  int m_default_value;
  int m_latch_cycles;
  int m_latch_count;
  int m_value;

};
