/*******************************************************************************
 *
 * File: RSpiGyro.cpp
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

#include "RobonautsLibrary/RSpiGyro.h"
#ifdef _2018
#include "DriverStation.h"
#include "LiveWindow/LiveWindow.h"
#include "Timer.h"
#else
#include "frc/DriverStation.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/Timer.h"
#endif
static constexpr double kSamplePeriod = 0.001;
static constexpr double kCalibrationSampleTime = 5.0;
static constexpr double kDegreePerSecondPerLSB = 0.0125;

static constexpr uint8_t kRateRegister = 0x00;
static constexpr uint8_t kTemRegister = 0x02;
static constexpr uint8_t kLoCSTRegister = 0x04;
static constexpr uint8_t kHiCSTRegister = 0x06;
static constexpr uint8_t kQuadRegister = 0x08;
static constexpr uint8_t kFaultRegister = 0x0A;
static constexpr uint8_t kPIDRegister = 0x0C;
static constexpr uint8_t kSNHighRegister = 0x0E;
static constexpr uint8_t kSNLowRegister = 0x10;

using namespace frc;

/**
 * Gyro constructor on the specified SPI port.
 *
 * @param port The SPI port the gyro is attached to.
 */
RSpiGyro::RSpiGyro(SPI::Port port, double sample_period, uint32_t spi_cmd, uint8_t xfer_size,
    uint32_t valid_mask, uint32_t valid_value, uint8_t data_shift, uint8_t data_size, bool is_signed,
    bool big_endian, bool invert)
    : m_spi(port)
{
    m_spi.SetClockRate(3000000);
    m_spi.SetMSBFirst();
#ifdef _2018
    m_spi.SetSampleDataOnRising();
#else
    m_spi.SetSampleDataOnLeadingEdge();
#endif
    m_spi.SetClockActiveHigh();
    m_spi.SetChipSelectActiveLow();

    m_invert = 1.0;

    m_is_ready = true;
    // Validate the part ID
    if ((ReadRegister(kPIDRegister) & 0xff00) != 0x5200)
    {
        DriverStation::ReportError("could not find ADXRS450 gyro");
        m_is_ready = false;
        return;
    }

    m_spi.InitAccumulator(sample_period, spi_cmd, xfer_size, valid_mask, valid_value,
        data_shift, data_size, is_signed, big_endian);

    //Calibrate();

    // add invert
    if(invert == true)
    {
    	m_invert = -1.0;
    }

//    HALReport(HALUsageReporting::kResourceType_ADXRS450, port);         // this is a defined type that is close
//    LiveWindow::GetInstance()->AddSensor("ADXRS450_Gyro", port, this);  // this is a known name that is close
}

/**
 * Initialize the gyro.
 * Calibrate the gyro by running for a number of samples and computing the
 * center value.
 * Then use the center value as the Accumulator center value for subsequent
 * measurements.
 * It's important to make sure that the robot is not moving while the centering
 * calculations are in progress, this is typically done when the robot is first
 * turned on while it's sitting at rest before the competition starts.
 */
void RSpiGyro::Calibrate()
{
    Wait(0.1);

    m_spi.SetAccumulatorCenter(0);
    m_spi.ResetAccumulator();

    Wait (kCalibrationSampleTime);

    m_spi.SetAccumulatorCenter((int32_t) m_spi.GetAccumulatorAverage());
    m_spi.ResetAccumulator();
}

static bool CalcParity(uint32_t v)
{
    bool parity = false;
    while (v != 0)
    {
        parity = !parity;
        v = v & (v - 1);
    }
    return parity;
}

static inline uint32_t BytesToIntBE(uint8_t* buf)
{
    uint32_t result = ((uint32_t) buf[0]) << 24;
    result |= ((uint32_t) buf[1]) << 16;
    result |= ((uint32_t) buf[2]) << 8;
    result |= (uint32_t) buf[3];
    return result;
}

uint16_t RSpiGyro::ReadRegister(uint8_t reg)
{
    uint32_t cmd = 0x80000000 | (((uint32_t) reg) << 17);
    if (!CalcParity(cmd))
        cmd |= 1u;

    // big endian
    uint8_t buf[4] =
    { (uint8_t)((cmd >> 24) & 0xff), (uint8_t)((cmd >> 16) & 0xff), (uint8_t)((cmd >> 8) & 0xff), (uint8_t)(cmd & 0xff) };

    m_spi.Write(buf, 4);
    m_spi.Read(false, buf, 4);
    if ((buf[0] & 0xe0) == 0)
        return 0;  // error, return 0
    return (uint16_t)((BytesToIntBE(buf) >> 5) & 0xffff);
}

/**
 * Reset the gyro.
 * Resets the gyro to a heading of zero. This can be used if there is
 * significant
 * drift in the gyro and it needs to be recalibrated after it has been running.
 */
void RSpiGyro::Reset()
{
    m_spi.ResetAccumulator();
}

/**
 * Return the actual angle in degrees that the robot is currently facing.
 *
 * The angle is based on the current accumulator value corrected by the
 * oversampling rate, the
 * gyro type and the A/D calibration values.
 * The angle is continuous, that is it will continue from 360->361 degrees. This
 * allows algorithms that wouldn't
 * want to see a discontinuity in the gyro output as it sweeps from 360 to 0 on
 * the second time around.
 *
 * @return the current heading of the robot in degrees. This heading is based on
 * integration
 * of the returned rate from the gyro.
 */
double RSpiGyro::GetAngle() const
{
    return (double) (m_invert * m_spi.GetAccumulatorValue() * kDegreePerSecondPerLSB * kSamplePeriod);
}

/**
 * Return the rate of rotation of the gyro
 *
 * The rate is based on the most recent reading of the gyro analog value
 *
 * @return the current rate in degrees per second
 */
double RSpiGyro::GetRate() const
{
    return (double) (m_invert * m_spi.GetAccumulatorLastValue() * kDegreePerSecondPerLSB);
}
