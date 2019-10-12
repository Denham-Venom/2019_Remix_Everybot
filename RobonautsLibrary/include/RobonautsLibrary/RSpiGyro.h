/*******************************************************************************
 *
 * File: RSpiGyro.h
 *
 * Written by:
 *  The Robonauts
 *  FRC Team 118
 *  NASA, Johnson Space Center
 *  Clear Creek Independent School District
 *
 * This file is based on the WPILib ADXRS450 Gyro class with minor
 * modifications to allow for more configuration and use of the ADXRS453 gyro
 * on the Spartan board.
 *
 ******************************************************************************/

#pragma once

#ifdef _2018
#include "GyroBase.h"
#include "Notifier.h"
#include "SPI.h"
#include "support/priority_mutex.h"
#else
#include "frc/GyroBase.h"
#include "frc/Notifier.h"
#include "frc/SPI.h"
#include "wpi/priority_mutex.h"
#endif

#include <memory>

using namespace frc;

/**
 * Use a rate gyro to return the robots heading relative to a starting position.
 * The Gyro class tracks the robots heading based on the starting position. As
 * the robot rotates the new heading is computed by integrating the rate of
 * rotation returned by the sensor. When the class is instantiated, it does a
 * short calibration routine where it samples the gyro while at rest to
 * determine the default offset. This is subtracted from each sample to
 * determine the heading.
 *
 * This class is for the digital ADXRS450 gyro sensor that connects via SPI.
 */
class RSpiGyro : public GyroBase
{
    public:
        explicit RSpiGyro(
            SPI::Port port=SPI::kMXP,
            double sample_period = 0.001,
            uint32_t spi_cmd = 0x20000000u,
            uint8_t xfer_size = 4,
            uint32_t valid_mask = 0x0c00000eu, // ADXRS450 is 0x0c000000u
            uint32_t valid_value = 0x04000000u,
            uint8_t data_shift = 10u,
            uint8_t data_size = 16u,
            bool is_signed = true,
            bool big_endian = true,
			bool invert = false);

        virtual ~RSpiGyro() = default;

        double GetAngle() const override;
        double GetRate() const override;
        void Reset() override;
        void Calibrate() override;
        bool IsReady() { return( m_is_ready); };

    private:
        SPI m_spi;
        float m_invert;
        bool m_is_ready;

        uint16_t ReadRegister(uint8_t reg);
};
