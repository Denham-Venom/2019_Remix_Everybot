/*******************************************************************************
 *
 * File: GameDataControl.h
 *
 * This class is to control a gameData using TBD
 *
 * 
 * Written by:
 *     The Robonauts
 *     FRC Team 118
 *     NASA, Johnson Space Center
 *     Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "gsu/tinyxml2.h"

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/XmlRobotUtil.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"
#include "RobonautsLibrary/MacroStep.h"
#include "RobonautsLibrary/OIDriverStation.h"

/*******************************************************************************
 *
 * This control is for a gameData that is connected with TBD
 *
 * XML Example: TBD
 *<control type="GameData" name="GameDataControlWhatever" device_name="ds/driver_station/whatever it is called in interface section" />
 *
 ******************************************************************************/
class GameDataControl: public PeriodicControl, public OIObserver
{
    public:
        GameDataControl(std::string name, tinyxml2::XMLElement *xml = NULL);
        ~GameDataControl();
        OIDriverStation::AllianceColorDirection getZoneDirection(OIDriverStation::IntChannel channel);

        void publish(void);

    protected:
        void controlInit();
        void updateConfig();

        void disabledInit();
        void autonomousInit();
        void teleopInit();
        void testInit();

        void doPeriodic();

        void setAnalog(int, float);
        void setDigital(int, bool);
        void setInt(int, int);

    private:
        std::string m_game_data_message;

        OIDriverStation::AllianceColorDirection m_near_direction;
        OIDriverStation::AllianceColorDirection m_center_direction;
        OIDriverStation::AllianceColorDirection m_far_direction;
        std::string getStateString(OIDriverStation::AllianceColorDirection which);

};

/*******************************************************************************
 *
 * This class handles game data macro step for autons and the like
 *
 * XML Example: TBD
 *<control type="GameData" name="GameDataControlWhatever" device_name="ds/driver_station/whatever it is called in interface section" />
 *
 ******************************************************************************/
class GameDataZoneMacroStep : public MacroStep
{
	public:
		GameDataZoneMacroStep(std::string type, tinyxml2::XMLElement *xml, void *control);
		virtual ~GameDataZoneMacroStep();

		MacroStep * connect(std::string which, MacroStep * step);

		virtual void clear(void);
		virtual void init(void);
		virtual MacroStep * update(void);

	protected:
		MacroStep * m_left_step;
		MacroStep * m_right_step;
		MacroStep * m_invalid_step;

	private:
		GameDataControl * m_parent_control;
		OIDriverStation::IntChannel m_zone;
};
