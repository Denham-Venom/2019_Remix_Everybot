/*******************************************************************************
 *
 * File: PacbotBase.h
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
#include "IterativeRobot.h"
#include "smartdashboard/SendableChooser.h"
#else
#include "frc/TimedRobot.h"
#include "frc/smartdashboard/SendableChooser.h"
#endif

#include "RobonautsControls/ControlThread.h"
#include "RobonautsLibrary/MacroController.h"
#include "RobonautsLibrary/OIObserver.h"

/*******************************************************************************
 *
 * The main class for this robot controls the creation of the robot, loading
 * the configuration from file, and controlling the transition between
 * operation phases.
 * 
 ******************************************************************************/
#ifdef _2018
class PacbotBase : public IterativeRobot, public OIObserver
#else
class PacbotBase : public frc::TimedRobot, public OIObserver
#endif
{
    public:
        enum Phase
        {
            INIT, DISABLED, AUTON, TELEOP, TEST
        };

        enum Command
        {
            CMD_START_MACRO = 200, CMD_START_MACRO_MAX = 299,
            CMD_ABORT_MACRO = 300, CMD_ABORT_MACRO_MAX = 399
        };

        PacbotBase(void);
		virtual ~PacbotBase();

		virtual void RobotInit(void);
		virtual void LoadRobot(std::string file);
		virtual void UpdateConfig(void);
		virtual void UpdateAutonConfig(void);

		virtual void DisabledInit(void);
		virtual void DisabledPeriodic(void);

		virtual void AutonomousInit(void);
		virtual void AutonomousPeriodic(void);

		virtual void TeleopInit(void);
		virtual void TeleopPeriodic(void);

		virtual void TestInit(void);
		virtual void TestPeriodic(void);

		virtual void setAnalog(int id, float val);
		virtual void setDigital(int id, bool val);
		virtual void setInt(int id, int val);

    protected:
        virtual void RegisterControls(void);

	private:
        class AutonIndex
        {
            public:
                AutonIndex(int i) : index(i) {}
                int value(void) {return index;}
            private:
                int index;
        };

        virtual bool isAutonAvailable();
        virtual void publish(void);

        std::vector<ControlThread*> m_controls;

        MacroController *m_macro_controller;
		std::vector<std::string> m_macro_name_list;
		std::vector<std::string> m_macro_file_list;

        std::vector<std::string> m_auton_name_list;
        std::vector<std::string> m_auton_file_list;

        int         m_auton_file_count;
        int         m_auton_select;
        int         m_auton_select_loaded;
        int         m_auton_select_mismatch;
        bool        m_auton_enabled;
        float       m_auton_delay;
        bool        m_auton_delay_enabled;
        double      m_auton_start_time;
        bool        m_auton_started;
        std::string m_auton_description;
#ifdef _2018
	SendableChooser<AutonIndex *> m_auton_chooser;
#else
	frc::SendableChooser<AutonIndex *> m_auton_chooser;
#endif
	uint8_t m_publish_count;

        Phase m_robot_phase;
};
