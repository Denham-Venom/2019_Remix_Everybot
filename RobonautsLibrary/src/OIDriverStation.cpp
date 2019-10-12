/*******************************************************************************
 *
 * File: OIDriverStation.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#include "gsu/Advisory.h"

#include <RobonautsLibrary/OIDriverStation.h>

using namespace frc;

/*******************************************************************************
 *
 * Create an instance of a Driver Station. This class acts like a middle man
 * between the WPI DriverStation and OIController.
 *
 * @param	name	The name of the OIDriverStation.
 * @param	ds		The DriverStation OIDriverStation will communicate to.
 *
 ******************************************************************************/
OIDriverStation::OIDriverStation(std::string name, DriverStation &ds)
	: OIDevice(name, NUM_ANALOG_CHANNELS, NUM_DIGITAL_CHANNELS, NUM_INT_CHANNELS, NUM_STRING_CHANNELS)
	, m_driver_station(ds)
{
	Advisory::pinfo("Creating OI Driver Station %s", name.c_str());
}

/*******************************************************************************
 *
 * Release any resources allocated by an instance of this class.
 *
 ******************************************************************************/
OIDriverStation::~OIDriverStation()
{
}

/*******************************************************************************
 *
 * pull out the first byte from the 2017 message and set for game zone
 *
 ******************************************************************************/
OIDriverStation::AllianceColorDirection OIDriverStation::getGameZoneDirection(IntChannel channel)
{
    int index = 0;
    //Advisory::pinfo("getGameZoneDirection:: channel: %d message: -%s-", channel, m_driver_station.GetGameSpecificMessage().c_str() );

    // set the index into the string based on the channel selected
    if (channel == GAME_ZONE_NEAR)
    {
       index = 0;
    }
    else if (channel == GAME_ZONE_CENTER)
    {
        index = 1;
    }
    else if (channel == GAME_ZONE_FAR)
    {
        index = 2;
    }
    else
    {
        return ALLIANCE_COLOR_INVALID;
    }

    // check the length of the string for valid length
    if (m_driver_station.GetGameSpecificMessage().length() < 3)
    {
        return ALLIANCE_COLOR_INVALID;
    }

    if (m_driver_station.GetGameSpecificMessage()[index] == 'l' ||
        m_driver_station.GetGameSpecificMessage()[index] == 'L')
    {
        return ALLIANCE_COLOR_LEFT;
    }
    else if (m_driver_station.GetGameSpecificMessage()[index] == 'r' ||
             m_driver_station.GetGameSpecificMessage()[index] == 'R')
    {
        return ALLIANCE_COLOR_RIGHT;
    }
    else
    {
        return ALLIANCE_COLOR_INVALID;
    }

}

/*******************************************************************************
 *
 * Check all of the input channels, if any of them changed notify the
 * observer of the change.
 *
 ******************************************************************************/
void OIDriverStation::update(void)
{
	updateAnalog(MATCH_TIME, m_driver_station.GetMatchTime());
	updateAnalog(BATTERY_VOLTAGE, m_driver_station.GetBatteryVoltage());

	updateDigital(IS_ENABLED, m_driver_station.IsEnabled());
	updateDigital(IS_AUTON, m_driver_station.IsAutonomous());
	updateDigital(IS_TELEOP, m_driver_station.IsOperatorControl());
	updateDigital(IS_TEST, m_driver_station.IsTest());
	updateDigital(IS_DS_ATTACHED, m_driver_station.IsDSAttached());
	updateDigital(IS_FMS_ATTACHED, m_driver_station.IsEnabled());
	updateDigital(IS_SYS_ACTIVE, RobotController::IsSysActive());
	updateDigital(IS_SYS_BROWNED_OUT, RobotController::IsBrownedOut());
//	updateDigital(IS_SYS_ACTIVE, m_driver_station.IsSysActive());
//	updateDigital(IS_SYS_BROWNED_OUT, m_driver_station.IsBrownedOut());

	updateInt(ALLIANCE, m_driver_station.GetAlliance());
	updateInt(LOCATION, m_driver_station.GetLocation());
	updateInt(GAME_ZONE_NEAR,   getGameZoneDirection(GAME_ZONE_NEAR));
	updateInt(GAME_ZONE_CENTER, getGameZoneDirection(GAME_ZONE_CENTER));
	updateInt(GAME_ZONE_FAR,    getGameZoneDirection(GAME_ZONE_FAR));

    updateString(EVENT_NAME,  m_driver_station.GetEventName());
    updateString(GAME_MESSAGE, m_driver_station.GetGameSpecificMessage());
}
