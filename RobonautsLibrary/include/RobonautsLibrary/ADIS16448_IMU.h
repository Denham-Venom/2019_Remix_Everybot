/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifdef _2018
#include "DigitalOutput.h"
#include "DigitalSource.h"
#include "GyroBase.h"
#include "InterruptableSensorBase.h"
#include "SPI.h"
#include "support/priority_condition_variable.h"
#include "support/priority_mutex.h"
#else
#include "frc/DigitalOutput.h"
#include "frc/DigitalSource.h"
#include "frc/GyroBase.h"
#include "frc/InterruptableSensorBase.h"
#include "frc/SPI.h"
// BEGINNING 2019 #include "wpi/priority_condition_variable.h"
#include "wpi/priority_mutex.h"

#endif

#include <atomic>

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

using namespace frc;

/**
 * This class is for the ADIS16448 IMU that connects to the RoboRIO MXP port.
 */
class ADIS16448_IMU : public GyroBase
{
 public:
  enum AHRSAlgorithm { kComplementary, kMadgwick };
  enum Axis { kX, kY, kZ };

  ADIS16448_IMU(Axis yaw_axis = kZ, AHRSAlgorithm algorithm = kComplementary);
  ~ADIS16448_IMU();

  void Calibrate() override;
  void Reset() override;
  double GetAngle() const override;
  double GetRate() const override;
  double GetAngleX() const;
  double GetAngleY() const;
  double GetAngleZ() const;
  double GetRateX() const;
  double GetRateY() const;
  double GetRateZ() const;
  double GetAccelX() const;
  double GetAccelY() const;
  double GetAccelZ() const;
  double GetMagX() const;
  double GetMagY() const;
  double GetMagZ() const;
  double GetPitch() const;
  double GetRoll() const;
  double GetYaw() const;
  double GetLastSampleTime() const;
  double GetBarometricPressure() const;
  double GetTemperature() const;
  double GetQuaternionW() const;
  double GetQuaternionX() const;
  double GetQuaternionY() const;
  double GetQuaternionZ() const;
  void SetTiltCompYaw(bool enabled);

  void UpdateTable();

 private:
  // Sample from the IMU
  struct Sample {
    double gyro_x;
    double gyro_y;
    double gyro_z;
    double accel_x;
    double accel_y;
    double accel_z;
    double mag_x;
    double mag_y;
    double mag_z;
    double baro;
    double temp;
    double dt;

    // Swap axis as appropriate for yaw axis selection
    void AdjustYawAxis(Axis yaw_axis);
  };

  uint16_t ReadRegister(uint8_t reg);
  void WriteRegister(uint8_t reg, uint16_t val);
  void Acquire();
  void Calculate();
  void CalculateMadgwick(Sample& sample, double beta);
  void CalculateComplementary(Sample& sample);

  // AHRS algorithm
  AHRSAlgorithm m_algorithm;

  // AHRS yaw axis
  Axis m_yaw_axis;

  // serial number and lot id
  //int m_serial_num;
  //int m_lot_id1;
  //int m_lot_id2;

  // gyro offset
  double m_gyro_offset_x = 0.0;
  double m_gyro_offset_y = 0.0;
  double m_gyro_offset_z = 0.0;

  // last read values (post-scaling)
  double m_gyro_x = 0.0;
  double m_gyro_y = 0.0;
  double m_gyro_z = 0.0;
  double m_accel_x = 0.0;
  double m_accel_y = 0.0;
  double m_accel_z = 0.0;
  double m_mag_x = 0.0;
  double m_mag_y = 0.0;
  double m_mag_z = 0.0;
  double m_baro = 0.0;
  double m_temp = 0.0;

  // accumulated gyro values (for offset calculation)
  int m_accum_count = 0;
  double m_accum_gyro_x = 0.0;
  double m_accum_gyro_y = 0.0;
  double m_accum_gyro_z = 0.0;

  // integrated gyro values
  double m_integ_gyro_x = 0.0;
  double m_integ_gyro_y = 0.0;
  double m_integ_gyro_z = 0.0;

  // last sample time
  double m_last_sample_time = 0.0;

  // Kalman (AHRS)
  double m_ahrs_q1 = 1, m_ahrs_q2 = 0, m_ahrs_q3 = 0, m_ahrs_q4 = 0;

  // Complementary AHRS
  bool m_first = true;
  double m_gyro_x_prev;
  double m_gyro_y_prev;
  double m_gyro_z_prev;
  double m_mag_angle_prev = 0.0;
  bool m_tilt_comp_yaw = true;

  // AHRS outputs
  double m_yaw = 0.0;
  double m_roll = 0.0;
  double m_pitch = 0.0;

  std::atomic_bool m_freed;

  SPI m_spi;
  std::unique_ptr<DigitalOutput> m_reset;
  std::unique_ptr<DigitalSource> m_interrupt;

  std::thread m_acquire_task;
  std::thread m_calculate_task;

  mutable wpi::priority_mutex m_mutex;

  // Samples FIFO.  We make the FIFO 2 longer than it needs
  // to be so the input and output never overlap (we hold a reference
  // to the output while the lock is released).
  static constexpr int kSamplesDepth = 10;
  Sample m_samples[kSamplesDepth + 2];

  wpi::priority_mutex m_samples_mutex;
// BEGINNING 2019   wpi::priority_condition_variable m_samples_not_empty;

  int m_samples_count = 0;
  int m_samples_take_index = 0;
  int m_samples_put_index = 0;
  bool m_calculate_started = false;
};
