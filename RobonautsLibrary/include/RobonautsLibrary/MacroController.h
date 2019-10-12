/*******************************************************************************
 *
 * File: MacroController.h
 * 
 * This file contains the definition of a class that holds and controls the
 * execution of a collection of Macros.
 *
 * @see MacroStepFactory.h for more information about Macros
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <vector>
#include <map>
#include "gsi/PeriodicThread.h"

#include "RobonautsLibrary/Macro.h"

/*******************************************************************************
 *
 * The MacroController class has static methods designed to manage
 * the operation of a list of Macros.  Macros can be added to the list,
 * run, and terminated.
 * *
 ******************************************************************************/
class MacroController:public gsi::PeriodicThread
{
	public:
		MacroController(void);
		~MacroController(void);

		void startMacro(Macro *macro);
		void abortMacro(Macro *macro);
		void putMacro(std::string name, Macro *macro);
		void loadMacro(std::string name, std::string filename /*, RobotMain *robot_main*/);
		
		Macro * getMacro(std::string name);
		void startMacro(std::string macro_name);
		void abortMacro(std::string macro_name);
		void abortAll();

		bool isRunning(std::string macro_name);

		uint32_t getMacroStarts(void);
		uint32_t getMacroCompletes(void);
		uint32_t getMacroAborts(void);
		uint32_t getMacrosRunning(void);

	protected:
		virtual void doPeriodic(void);

	private:
		std::vector<Macro *> macro_active_list;
		std::vector<Macro *> macro_start_list;
		std::vector<Macro *> macro_end_list;

		std::map<std::string, Macro *> macro_available_map;

		uint32_t macro_starts;
		uint32_t macro_completes;
		uint32_t macro_aborts;
};
