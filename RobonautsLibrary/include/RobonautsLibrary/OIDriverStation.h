/*******************************************************************************
 *
 * File: OIDriverStation.h
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#ifdef _2018
#include "DriverStation.h"
#include "RobotController.h"
#else
#include "frc/DriverStation.h"
#include "frc/RobotController.h"
#endif

#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIDevice.h"

using namespace frc;

class OIDriverStation : public OIDevice
{
	public:
		enum AnalogChannel
		{
			MATCH_TIME = 0, BATTERY_VOLTAGE, NUM_ANALOG_CHANNELS
		};

		enum DigitalChannel
		{
			IS_ENABLED = 0, IS_AUTON, IS_TELEOP, IS_TEST, IS_DS_ATTACHED,
			IS_FMS_ATTACHED, IS_SYS_ACTIVE, IS_SYS_BROWNED_OUT,
			NUM_DIGITAL_CHANNELS
		};

		enum IntChannel
		{
			ALLIANCE = 0, LOCATION, GAME_ZONE_NEAR, GAME_ZONE_CENTER, GAME_ZONE_FAR, NUM_INT_CHANNELS
		};

                enum StringChannel
                {
                      EVENT_NAME = 0, GAME_MESSAGE, NUM_STRING_CHANNELS
                };

                enum AllianceColorDirection
                {
                    ALLIANCE_COLOR_LEFT = 0, ALLIANCE_COLOR_RIGHT, ALLIANCE_COLOR_INVALID, NUM_ALLIANCE_COLOR_DIRECTIONS
                };

		OIDriverStation(std::string name, DriverStation &ds);

		virtual ~OIDriverStation();

		void update(void);
                AllianceColorDirection getGameZoneDirection(IntChannel);

	private:
		DriverStation  &m_driver_station;

};
