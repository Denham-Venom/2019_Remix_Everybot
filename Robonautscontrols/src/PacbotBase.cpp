/*******************************************************************************
 *
 * File: PacbotBase.cpp
 *
 * Written by:   
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string>
#include <iostream>

//#include "frc/WPILib.h"

#include "gsu/tinyxml2.h"
#include "gsu/Advisory.h"
#include "gsu/AdvisoryFile.h"
#include "gsu/AdvisoryStdOut.h"
#include "gsu/SegmentedFile.h"

#include "RobonautsLibrary/Macro.h"
#include "RobonautsLibrary/MacroController.h"
#include "RobonautsLibrary/OIController.h"

#include "RobonautsControls/ControlFactory.h"

#include "RobonautsControls/ArcadeDriveControl.h"
#include "RobonautsControls/KiwiDriveControl.h"
#include "RobonautsControls/CameraServerControl.h"
#include "RobonautsControls/CompressorControl.h"
#include "RobonautsControls/MotorControl.h"
#include "RobonautsControls/RelayControl.h"
#include "RobonautsControls/SolenoidControl.h"
#include "RobonautsControls/TurretControl.h"
#include "RobonautsControls/GameDataControl.h"
#include "RobonautsControls/PositionJointSRX.h"

#include "RobonautsControls/ServoControl.h"

#include "RobonautsControls/PacbotBase.h"

#include "frc/liveWindow/LiveWindow.h"

using namespace tinyxml2;
using namespace gsu;
using namespace frc;

/******************************************************************************
 *
 * Creates a new OIController and loads the robot
 *
 ******************************************************************************/
PacbotBase::PacbotBase(void)
{
    Advisory::pinfo("Robot Constructor");

    m_macro_controller = nullptr;

    m_auton_file_count        = 0;
    m_auton_select            = 0;
    m_auton_select_mismatch   = -1;
    m_auton_select_loaded     = -1;
    m_auton_enabled           = true;
    m_auton_delay             = 0.0;
    m_auton_delay_enabled     = false;
    m_auton_start_time        = 0.0;
    m_auton_started           = false;
    m_auton_description       = "";

    m_publish_count = 0;

    m_robot_phase = INIT;
}

/******************************************************************************
 *
 * Deletes any controls in the destructor
 *
 ******************************************************************************/
PacbotBase::~PacbotBase()
{
    Advisory::pinfo("Robot Destructor");

	for (ControlThread *control :  m_controls )
	{
		delete control;
	}
	m_controls.clear();

	if (m_macro_controller != nullptr)
	{
		delete m_macro_controller;
		m_macro_controller = nullptr;
	}
}

/******************************************************************************
 *
 * Register the controls that can be used by this robot
 *
 ******************************************************************************/
void PacbotBase::RegisterControls(void)
{
    Advisory::pinfo("Robot Register Controls");

    ControlFactory::registerProxy("arcade_drive", new ControlProxy<ArcadeDriveControl>());
    ControlFactory::registerProxy("kiwi_drive", new ControlProxy<KiwiDriveControl>());
    ControlFactory::registerProxy("camera_server", new ControlProxy<CameraServerControl>());
    ControlFactory::registerProxy("compressor", new ControlProxy<CompressorControl>());
    ControlFactory::registerProxy("motor", new ControlProxy<MotorControl>());
    ControlFactory::registerProxy("solenoid", new ControlProxy<SolenoidControl>());
    ControlFactory::registerProxy("turret", new ControlProxy<TurretControl>());
    ControlFactory::registerProxy("relay", new ControlProxy<RelayControl>());
    ControlFactory::registerProxy("servo", new ControlProxy<ServoControl>());
    ControlFactory::registerProxy("game_data", new ControlProxy<GameDataControl>());
    ControlFactory::registerProxy("pjs", new ControlProxy<PositionJointSRX>());

}

/******************************************************************************
 *
 * Initializes the robot and controls
 *
 ******************************************************************************/
void PacbotBase::RobotInit(void)
{
    LiveWindow::GetInstance()->DisableAllTelemetry();

    Advisory::addObserver(new AdvisoryStdOut());
    Advisory::addObserver(new AdvisoryFile(SegmentedFile("/robot/logs/advisories", "message", "txt", 10).openNextSegment()));

    Advisory::pinfo("Robot RobotInit()");

    m_macro_controller = new MacroController();
    m_macro_controller->start();

    LoadRobot("/robot/config/RobotControl.xml");

	for (ControlThread *control :  m_controls )
	{
		control->doControlInit();
	}

	for (ControlThread *control :  m_controls )
	{
		control->start();
	}
}

/******************************************************************************
 *
 * Loads the XML file and calls each class accordingly
 *
 ******************************************************************************/
void PacbotBase::LoadRobot(std::string file)
{
	XMLDocument doc;
	XMLError err;
	err = doc.LoadFile(file.c_str());

	if(err != XML_SUCCESS)
	{
		Advisory::pwarning("could not read macro file %s", file.c_str());
		return;
	}

	XMLElement *robot = doc.FirstChildElement("robot");
    Advisory::pinfo("############################## Loading Robot ##############################");

	if (robot == nullptr)
	{
	    Advisory::pinfo("ERROR: could not read config file %s, no \"robot\" element",
			file.c_str());
		return;
	}
	
	const char *name = robot->Attribute("name");
	if (name != nullptr)
	{
	    Advisory::pinfo("Robot LoadRobot - %s", name);
	}
	else
	{
	    Advisory::pinfo("Robot LoadRobot - unnamed");
	}

    Advisory::pinfo("========================= Loading User Interface =========================");
	XMLElement *oi_xml = robot->FirstChildElement("interface");
	if (oi_xml != nullptr)
	{
		OIController::configure(oi_xml);
	}
	else
	{
	    Advisory::pinfo("ERROR: could not initialize OIController, no xml element named \"interface\"");
		return;
	}
	
    Advisory::pinfo("========================= Loading Robot Controls =========================");
	XMLElement *control_xml = robot->FirstChildElement("control");
	while (control_xml != nullptr)
	{
		const char* type = control_xml->Attribute("type");
		const char *name = control_xml->Attribute("name");
		
		if (name == nullptr)
		{
			name = "unnamed";
		}
		
		if (type != nullptr)
		{
		    Advisory::pinfo("Create %s - %s", type, name);
			ControlThread *control = ControlFactory::create(type, control_xml);

			if (control != nullptr)
			{
				m_controls.push_back(control);
			}
			else
			{
			    Advisory::pinfo("ERROR: failed to create control %s - %s, make sure the %s type is registered with the factory\n", type, name, type);
			}
		}
		else
		{
		    Advisory::pinfo("ERROR: could not create control, type is NULL");
		}

		control_xml = control_xml->NextSiblingElement("control");
	}

    Advisory::pinfo("==================== Loading Auton File Names ====================");
    XMLElement *auton_elem = robot->FirstChildElement("auton");
    while (auton_elem != nullptr)
    {
        const char *auton_name = auton_elem->Attribute("name");
        const char *auton_file = auton_elem->Attribute("file");

        if ((auton_name != nullptr) && (auton_file != nullptr))
        {
            if (0 == m_auton_name_list.size())
            {
#ifdef _2018 
                // 2019: AddDefault is depricated, recommends changing to SetDefaultOption 
                m_auton_chooser.AddDefault(auton_name, new AutonIndex(m_auton_name_list.size()));
#else
                m_auton_chooser.SetDefaultOption(auton_name, new AutonIndex(m_auton_name_list.size())); 
#endif
            }
            else
            {
#ifdef _2018 
               // 2019: AddDefault is depricated, recommends changing to SetDefaultOption 
                m_auton_chooser.AddObject(auton_name, new AutonIndex(m_auton_name_list.size()));
#else
                m_auton_chooser.AddOption(auton_name, new AutonIndex(m_auton_name_list.size()));
#endif
            }
            m_auton_name_list.push_back(auton_name);
            m_auton_file_list.push_back(auton_file);
        }
        else
        {
            Advisory::pwarning("could not parse auton tag");
        }
        auton_elem = auton_elem->NextSiblingElement("auton");
    }

    m_auton_file_count = m_auton_name_list.size();
    Advisory::pinfo("found %d named auton files", m_auton_file_count);

    if (isAutonAvailable())
    {
        m_auton_enabled = true;

        SmartDashboard::PutData("Auton Selected: ", &m_auton_chooser);
        SmartDashboard::PutBoolean("Auton Enabled: ", m_auton_enabled);
        SmartDashboard::PutBoolean("Auton Delay Enabled: ", false);
        SmartDashboard::PutNumber("Auton Delay: ", 0.0);
        SmartDashboard::PutString("Auton Description: ", "none");
    }

    Advisory::pinfo("====================== Loading Macros Names ========================");
	XMLElement *macro_xml = robot->FirstChildElement("macro");
	while (macro_xml != nullptr)
	{
		const char *macro_name = macro_xml->Attribute("name");
		const char *macro_file = macro_xml->Attribute("file");

		if ((macro_name != nullptr) && (macro_file != nullptr))
		{
            // Don't load them here, they will be loaded in UpdateConfig()
			// m_macro_controller->loadMacro(macro_name, macro_file, this);

			XMLElement *oi_xml = macro_xml->FirstChildElement("oi");
			while (oi_xml != nullptr)
			{
                name = oi_xml->Attribute("name");
                if (name != NULL)
                {
                    if (strcmp(name, "start") == 0)
                    {
                        Advisory::pinfo("  connecting start macro %s to oi button", macro_name);
                        OIController::subscribeDigital(oi_xml, this, CMD_START_MACRO + m_macro_name_list.size());
                    }
                    else if (strcmp(name, "abort") == 0)
                    {
                        Advisory::pinfo("  connecting abort macro %s to oi button", macro_name);
                       OIController::subscribeDigital(oi_xml, this, CMD_ABORT_MACRO + m_macro_name_list.size());
                    }
                }
				oi_xml = oi_xml->NextSiblingElement("oi");
			}

			m_macro_name_list.push_back(macro_name);
			m_macro_file_list.push_back(macro_file);
		}
		else
		{
		    Advisory::pinfo("ERROR: could not load macro file");
		}
		macro_xml = macro_xml->NextSiblingElement("macro");
	}

    Advisory::pinfo("========================== Loading Macros ==========================");
    int macro_index = 0;
    for (std::string file : m_macro_file_list)
    {
        m_macro_controller->loadMacro(m_macro_name_list[macro_index].c_str(), file /*, this*/);
        macro_index++;
    }

    UpdateAutonConfig();

    Advisory::pinfo("############################## Robot Loaded  ##############################");
}

/*******************************************************************************
 *
 * Called during initialization to update the configuration.
 *
 * In previous version of robot main, specific button sequences during
 * disabled could result in the reloading of certain file by calling this
 * method. For now that is mostly handled by checking to see if the
 * selected auton changed.
 *
 * This method may be deleted in the future.
 *
 ******************************************************************************/
void PacbotBase::UpdateConfig(void)
{
    Advisory::pinfo("Robot UpdateConfig()");

}

/*******************************************************************************
 *
 * This method is called during initialization to load the default auton
 * file if any. It will also be called during the disabled period if the
 * selected auton changed.
 *
 ******************************************************************************/
void PacbotBase::UpdateAutonConfig(void)
{
    try
    {
        if (isAutonAvailable())
        {
            Advisory::pinfo("Robot UpdateAutonConfig()");

            AutonIndex * auton_index = m_auton_chooser.GetSelected();
            if (auton_index != nullptr)
            {
                m_auton_select = auton_index->value();
            }

            m_auton_enabled = SmartDashboard::GetBoolean("Auton Enabled: ", m_auton_enabled);
            m_auton_delay_enabled = SmartDashboard::GetBoolean("Auton Delay Enabled: ", m_auton_delay_enabled);
            m_auton_delay = SmartDashboard::GetNumber("Auton Delay: ", m_auton_delay);

            Advisory::pinfo("RobotMain::UpdateAutonConfig -- enabled=%s, select=%d, delay_enabled=%s, delay=%5.3f",
                    m_auton_enabled?"true":"false", m_auton_select, m_auton_delay_enabled?"true":"false", m_auton_delay);

            if ((m_auton_select >= 0) && (m_auton_select < m_auton_file_count))
            {
                m_macro_controller->loadMacro("auton", m_auton_file_list[m_auton_select] /*, this*/);
                m_auton_select_loaded = m_auton_select;
                m_auton_select_mismatch = 0;
                m_auton_description = m_macro_controller->getMacro("auton")->getDescription();
            }
            else
            {
                m_auton_enabled = false;
                m_auton_select = -1;
                m_auton_select_loaded = -1;
                m_auton_description = "none";
                m_auton_delay_enabled = false;
                m_auton_delay = 0.0;

                SmartDashboard::PutBoolean("Auton Enabled: ", m_auton_enabled);
                SmartDashboard::PutBoolean("Auton Delay Enabled: ", m_auton_delay_enabled);
                SmartDashboard::PutNumber("Auton Delay: ", m_auton_delay);
           }

            SmartDashboard::PutString("Auton Description: ", m_auton_description);
        }
    }
    catch (...)
    {
        Advisory::pwarning("exception caught in PacbotBase::UpdateAutonConfig");
    }
}

/******************************************************************************
 *
 * Called one when the robot becomes Disabled
 *
 ******************************************************************************/
void PacbotBase::DisabledInit(void)
{
    m_robot_phase = DISABLED;
    Advisory::pinfo("Robot DisabledInit");

	m_macro_controller->abortAll();

	for (ControlThread *control :  m_controls )
	{
		control->doDisabledInit();
	}
}

/******************************************************************************
 *
 * Called repeatedly while the robot is Disabled
 *
 ******************************************************************************/
void PacbotBase::DisabledPeriodic(void)
{
	OIController::update();

    if (isAutonAvailable())
    {
        AutonIndex * auton_index = m_auton_chooser.GetSelected();
        if (auton_index != nullptr)
        {
            m_auton_select = auton_index->value();
        }

        if ((m_auton_select != m_auton_select_loaded) && (m_auton_select >= 0)
            && (m_auton_select < m_auton_file_count))
        {
            // 40 cycles should be about 2 seconds, that should be enough
            // time for the user to finish adjusting the auton selection
            if(m_auton_select_mismatch++ > 40)
            {
                UpdateAutonConfig();
            }
        }
    }

	publish();
}

/******************************************************************************
 *
 * Called once when the robot enters the Autonomous mode of operation
 *
 ******************************************************************************/
void PacbotBase::AutonomousInit(void)
{
    m_robot_phase = AUTON;
    Advisory::pinfo("Robot AutonomousInit");

    m_macro_controller->abortAll();

    if (isAutonAvailable())
    {
        m_auton_enabled = SmartDashboard::GetBoolean("Auton Enabled: ", m_auton_enabled);
        m_auton_delay_enabled = SmartDashboard::GetBoolean("Auton Delay Enabled: ", m_auton_delay_enabled);
        m_auton_delay = SmartDashboard::GetNumber("Auton Delay: ", m_auton_delay);

        m_auton_started = false;
        if (m_auton_enabled)
        {
            m_auton_start_time = GetTime();

            if (m_auton_delay_enabled)
            {
                m_auton_start_time += m_auton_delay;
            }
        }
//
//        Advisory::pinfo("RobotMain::AutonomousInit -- enabled=%s, select=%d, delay_enabled=%s, delay=%5.3f, start_time=%f, now=%f",
//                m_auton_enabled?"true":"false", m_auton_select, m_auton_delay_enabled?"true":"false", m_auton_delay,
//                		m_auton_start_time, GetTime());
//
    }

    for (ControlThread *control :  m_controls )
	{
		control->doAutonomousInit();
	}
}

/******************************************************************************
 *
 * Called repeatedly while the robot is in the Autonomous mode of operation
 *
 ******************************************************************************/
void PacbotBase::AutonomousPeriodic(void)
{
	OIController::update();

    if ((m_auton_enabled) && (GetTime() >= m_auton_start_time) && (! m_auton_started))
    {
        m_macro_controller->startMacro("auton");
        m_auton_started = true;
    }


    publish();
}

/******************************************************************************
 *
 * Called once when the robot enters the Teleoperated mode of operation
 *
 ******************************************************************************/
void PacbotBase::TeleopInit(void)
{
    m_robot_phase = TELEOP;
    Advisory::pinfo("Robot TeleopInit");

	for (ControlThread *control :  m_controls )
	{
		control->doTeleopInit();
	}
}

/******************************************************************************
 *
 * Called repeatedly while the robot is in the Teleoperated mode of operation
 *
 ******************************************************************************/
void PacbotBase::TeleopPeriodic(void)
{
	OIController::update();
	publish();
}

/******************************************************************************
 *
 * Called once when the robot enters the Test mode of operation
 *
 ******************************************************************************/
void PacbotBase::TestInit(void)
{
    m_robot_phase = TEST;
    Advisory::pinfo("Robot TestInit");

	for (ControlThread *control :  m_controls )
	{
		control->doTestInit();
	}
}

/******************************************************************************
 *
 * Called repeatedly while the robot is in the Test mode of operation
 *
 ******************************************************************************/
void PacbotBase::TestPeriodic(void)
{
	OIController::update();
	publish();
}

/*******************************************************************************
 *
 * Called only if an analog input is connected to this object during the
 * loading of the robot. At this time that does not happen so this will
 * not be called.
 *
 * Required by OIObserver
 *
 ******************************************************************************/
void PacbotBase::setAnalog(int id, float val)
{
//    try
//    {
//        switch(id)
//        {
//            default:
//            {
                Advisory::pinfo("Robot setFloat(%d, %f) -- no case for id", id, val);
//            } break;
//        }
 //   }
 //   catch (...)
 //   {
 //       Advisory::pinfo("EXCEPTION: caught in RobotMain::setFloat");
 //   }
}

/*******************************************************************************
 *
 *
 * Called only if a digital input is connected to this object during the
 * loading of the robot. That could happen if any macros are defined with
 * buttons that start/stop them.
 *
 * Required by OIObserver
 *
 *
 ******************************************************************************/
void PacbotBase::setDigital(int id, bool val)
{
    try
    {
//        switch(id)
//        {
//            default:
//            {
                if ((id >= CMD_START_MACRO) && (id < CMD_START_MACRO_MAX))
                {
                    if (val && (m_robot_phase != INIT) && (m_robot_phase != DISABLED))
                    {
                        m_macro_controller->startMacro(m_macro_name_list[id - CMD_START_MACRO]);
                    }
                }
                else if ((id >= CMD_ABORT_MACRO) && (id < CMD_ABORT_MACRO_MAX))
                {
                    if (val)
                    {
                        m_macro_controller->abortMacro(m_macro_name_list[id - CMD_ABORT_MACRO]);
                    }
                }
                else
                {
                    Advisory::pinfo("Robot setDigital(%d, %s) -- no case for id", id, (val?"true":"false"));
                }

//            } break;
//        }
    }
    catch (...)
    {
        Advisory::pinfo("EXCEPTION: caught in PacbotBase::setDigital");
    }
}

/*******************************************************************************
 *
 *
 * Called only if an integer input is connected to this object during the
 * loading of the robot. At this time that does not happen so this will
 * not be called.
 *
 * Required by OIObserver
 *
 *
 ******************************************************************************/
void PacbotBase::setInt(int id, int val)
{
//    try
//    {
//        switch(id)
//        {
//           default:
//           {
                Advisory::pinfo("Robot setInt(%d, %d) -- no case for id", id, val);
//           } break;
//        }
//    }
//    catch (...)
//    {
//        Advisory::pinfo("EXCEPTION: caught in RobotMain::setInt");
//    }
}

/******************************************************************************
 *
 * return true if at least one auton file was loaded
 *
 ******************************************************************************/
inline bool PacbotBase::isAutonAvailable()
{
    return m_auton_file_count > 0;
}

/******************************************************************************
 *
 * This method is called from within each of the periodic methods to allow
 * this class and all controls to publish values to the dashboard.
 *
 * Because the periodic methods should be getting called about 20 times a
 * second and the dashboard only needs to be updated every half second,
 * this method only does work every tenth time it is called.
 *
 ******************************************************************************/
void PacbotBase::publish(void)
{
    if (m_publish_count++ >= 20)
    {
        SmartDashboard::PutNumber("Macro Starts: ", this->m_macro_controller->getMacroStarts());
        SmartDashboard::PutNumber("Macro Completes: ", this->m_macro_controller->getMacroCompletes());
        SmartDashboard::PutNumber("Macro Aborts: ", this->m_macro_controller->getMacroAborts());
        SmartDashboard::PutNumber("Macros Running: ", this->m_macro_controller->getMacrosRunning());

        m_publish_count = 0;
    }

    if (m_publish_count < m_controls.size())
    {
        m_controls[m_publish_count]->doPublish();
    }
}

//
// This pre-processor macro is what makes this the class that is run
// as a robot main, this should be added to the end of any class that
// extends this class -- in this example the extending class is
// RobotMain.
//
//START_ROBOT_CLASS(RobotMain)
