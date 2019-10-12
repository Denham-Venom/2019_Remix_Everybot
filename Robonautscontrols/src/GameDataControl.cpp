/*******************************************************************************
 *
 * File: GameDataControl.cpp
 *
 * Written by:
 *     The Robonauts
 *     FRC Team 118
 *     NASA, Johnson Space Center
 *     Clear Creek Independent School District
 *
 ******************************************************************************/
#include <math.h>
#include <string>

#include "RobonautsLibrary/RobotUtil.h"

#include "gsu/Advisory.h"

#include "gsi/Time.h"

#include "RobonautsControls/GameDataControl.h"
#include "RobonautsLibrary/MacroStepFactory.h"

#ifdef _2018
#include "smartDashboard/SmartDashboard.h"
#else
#include "frc/smartDashboard/SmartDashboard.h"
#endif

using namespace tinyxml2;
using namespace gsi;
using namespace frc;

/******************************************************************************
 *
 * This is parsing the new control in RobotControl.xml
 * The controller name is game_data
 * In this instance, the type is a zone which can have multiple connects to
 * go left or right
 *
 *   <step type="Zone" ctrl_name="game_data" name="pickSide" zone="near">
 *  	<connect type="left"    step="intake_on"/>
 *  	<connect type="right"   step="intake_on_rev"/>
 *  	<connect type="invalid" step="intake_on_full"/>
 *  </step>
 *
 ******************************************************************************/
GameDataControl::GameDataControl(std::string control_name, tinyxml2::XMLElement *xml)
    : PeriodicControl(control_name)
{
    Advisory::pinfo("========================= Creating GameData Control [%s] =========================",
            control_name.c_str());

    setPeriod(1.0);

    m_game_data_message = "";

    m_near_direction   = OIDriverStation::ALLIANCE_COLOR_INVALID;
    m_center_direction = OIDriverStation::ALLIANCE_COLOR_INVALID;
    m_far_direction    = OIDriverStation::ALLIANCE_COLOR_INVALID;

	//
	// Register Macro Steps
	//
	new MacroStepProxy<GameDataZoneMacroStep>(control_name, "Zone", this);

    //
    // Parse the XML
    //
    const char * device_name = xml->Attribute("device_name");
	if (device_name != nullptr)
	{
		Advisory::pinfo("  connecting near channel");
		OIController::subscribeInt(device_name, OIDriverStation::GAME_ZONE_NEAR, this, OIDriverStation::GAME_ZONE_NEAR);

		Advisory::pinfo("  connecting center channel");
		OIController::subscribeInt(device_name, OIDriverStation::GAME_ZONE_CENTER, this, OIDriverStation::GAME_ZONE_CENTER);

		Advisory::pinfo("  connecting far channel");
		OIController::subscribeInt(device_name, OIDriverStation::GAME_ZONE_FAR, this, OIDriverStation::GAME_ZONE_FAR);
   	}
	else
	{
		Advisory::pwarning("  Failed to find \"device_name\" attribute");
	}
}

/******************************************************************************
 *
 * Destructor
 * 
 ******************************************************************************/
GameDataControl::~GameDataControl()
{
    Advisory::pinfo("GameDataSys::~GameDataSys");
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 * 
 ******************************************************************************/
void GameDataControl::controlInit()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void GameDataControl::updateConfig()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void GameDataControl::disabledInit()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void GameDataControl::autonomousInit()
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 *
 ******************************************************************************/
void GameDataControl::teleopInit()
{
}

/**********************************************************************
 *
 *
 **********************************************************************/
void GameDataControl::testInit(void)
{
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 * 
 ******************************************************************************/
void GameDataControl::publish()
{
    SmartDashboard::PutBoolean("  game data  ", (getCyclesSincePublish() > 0));
    SmartDashboard::PutNumber("game data cycles: ", getCyclesSincePublish());

    SmartDashboard::PutString("game data near: ", getStateString(m_near_direction));
    SmartDashboard::PutString("game data scale: ", getStateString(m_center_direction));
    SmartDashboard::PutString("game data far: ", getStateString(m_far_direction));
    SmartDashboard::PutNumber("match time: ", DriverStation::GetInstance().GetMatchTime());
}

/******************************************************************************
 *
 * Implements method required by PeriodicControl
 * 
 ******************************************************************************/
void GameDataControl::doPeriodic()
{
	// do nothing
}
OIDriverStation::AllianceColorDirection GameDataControl::getZoneDirection(OIDriverStation::IntChannel gamezone)
{
	switch (gamezone)
	{
		case OIDriverStation::GAME_ZONE_NEAR:   return m_near_direction; break;
		case OIDriverStation::GAME_ZONE_CENTER: return m_center_direction; break;
		case OIDriverStation::GAME_ZONE_FAR:    return m_far_direction; break;
		default: return OIDriverStation::ALLIANCE_COLOR_INVALID;
	}
}
/*******************************************************************************
 *
 ******************************************************************************/
void GameDataControl::setAnalog(int id, float val)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void GameDataControl::setDigital(int id, bool val)
{
}

/*******************************************************************************
 *  callback called if FMS software changes the Game Data string
 ******************************************************************************/
void GameDataControl::setInt(int id, int val)
{
	std::string idString = "";
	std::string valString = "";

	if (val == 0) valString = "LEFT";
	else if (val == 1 ) valString = "RIGHT";
	else valString = "INVALID";

	switch (id)
	{
	    case OIDriverStation::GAME_ZONE_NEAR:
	    	m_near_direction = (OIDriverStation::AllianceColorDirection) val;
	    	idString = "GAME_ZONE_NEAR  ";
	    	break;
	    case OIDriverStation::GAME_ZONE_CENTER:
	    	m_center_direction = (OIDriverStation::AllianceColorDirection) val;
	    	idString = "GAME_ZONE_CENTER";
	    	break;
	    case OIDriverStation::GAME_ZONE_FAR:
	    	m_far_direction = (OIDriverStation::AllianceColorDirection) val;
	    	idString = "GAME_ZONE_FAR   ";
	    	break;
	    default: break;
	}

	Advisory::pinfo("%s: GameDataControl::setInt()  id: %s (%d) val: %s (%d)", Time::getTimeString(Time::FORMAT_YMDHMSu).c_str(), idString.c_str(), id, valString.c_str(), val);

}

/*******************************************************************************
 *  used to write state of switches and scale to dashboard
 ******************************************************************************/
std::string GameDataControl::getStateString(OIDriverStation::AllianceColorDirection which)
{
	std::string state="";
    switch(which)
    {
    case OIDriverStation::AllianceColorDirection::ALLIANCE_COLOR_LEFT: state="Left"; break;
    case OIDriverStation::AllianceColorDirection::ALLIANCE_COLOR_RIGHT: state="Right"; break;
    case OIDriverStation::AllianceColorDirection::ALLIANCE_COLOR_INVALID: state="Invalid"; break;
    default: break;
    }
    return state;

}
/*******************************************************************************
 * Here begins the code to handle a Macro Step
 ******************************************************************************/
GameDataZoneMacroStep::GameDataZoneMacroStep(std::string type, tinyxml2::XMLElement *xml, void *control):
	MacroStep(type, xml, control)
{
	m_left_step = NULL;
	m_right_step = NULL;
	m_invalid_step = NULL;

	m_zone = OIDriverStation::GAME_ZONE_NEAR;
	m_parent_control = (GameDataControl *)control;
	const char* zone = xml->Attribute ("zone");
	if (zone != nullptr)
	{
		if (strcmp(zone,"near") == 0)
		{
		    m_zone = OIDriverStation::GAME_ZONE_NEAR;
		}
		else if (strcmp(zone,"center") == 0)
		{
			m_zone = OIDriverStation::GAME_ZONE_CENTER;
		}
		else if (strcmp(zone,"far") == 0)
		{
			m_zone = OIDriverStation::GAME_ZONE_FAR;
		}
		else
		{
			Advisory::pwarning ("invalid zone specified");
		}
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
GameDataZoneMacroStep::~GameDataZoneMacroStep()
{
	if (m_left_step != NULL)
	{
		delete m_left_step;
		m_left_step = NULL;
	}

	if (m_right_step != NULL)
	{
		delete m_right_step;
		m_right_step = NULL;
	}

	if (m_invalid_step != NULL)
	{
		delete m_invalid_step;
		m_invalid_step = NULL;
	}
}

/*******************************************************************************
 * used to initially connect steps together
 ******************************************************************************/
MacroStep * GameDataZoneMacroStep::connect(std::string which, MacroStep * step)
{
	if (which.compare("left") == 0)
	{
		m_left_step = step;
		if (m_left_step != NULL)
		{
			m_left_step->setParentMacro(parent_macro);
		}
		return m_left_step;
	}
	else if (which.compare("right") == 0)
	{
		m_right_step = step;
		if (m_right_step != NULL)
		{
			m_right_step->setParentMacro(parent_macro);
		}
		return m_right_step;
	}
	else if (which.compare("invalid") == 0)
	{
		m_invalid_step = step;
		if (m_invalid_step != NULL)
		{
			m_invalid_step->setParentMacro(parent_macro);
		}
		return m_invalid_step;
	}

	Advisory::pinfo(
	    "GameDataZone::connect invalid connection of %s for %s ignored",
	    which.c_str(), step_type.c_str());

	return NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
void GameDataZoneMacroStep::clear(void)
{
	if (! is_clear)
	{
		is_clear = true; // set is_clear before calling connections to
		            	 // stop a recursive loop

		if (m_left_step != NULL)
		{
			m_left_step->clear();
		}
		if (m_right_step != NULL)
		{
			m_right_step->clear();
		}
		if (m_invalid_step != NULL)
		{
			m_invalid_step->clear();
		}
	}
}

/*******************************************************************************
 * init gets called when the step first begins and sets up any initial data
 *
 ******************************************************************************/
void GameDataZoneMacroStep::init(void)
{
}

/*******************************************************************************
 * update gets called every cycle until returns something other than "this"
 * "this" is the same class thus would be called again, but a non-this pointer
 * would be the next step to execute
 ******************************************************************************/
MacroStep * GameDataZoneMacroStep::update(void)
{
	OIDriverStation::AllianceColorDirection side = m_parent_control->getZoneDirection(m_zone);
	if (side == OIDriverStation::ALLIANCE_COLOR_LEFT)
	{
		return m_left_step;
	}
	else if (side == OIDriverStation::ALLIANCE_COLOR_RIGHT)
	{
		return m_right_step;
	}
	else
	{
		return m_invalid_step;
	}

}
