/*******************************************************************************
 *
 * File: MacroController.cpp
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
#include "gsu/Advisory.h"

#include "RobonautsLibrary/DataLogger.h"
#include "RobonautsLibrary/MacroController.h"
#include "RobonautsLibrary/MacroStep.h"
#include "RobonautsLibrary/MacroStepFactory.h"

#include <algorithm>    // std::find

using namespace std;

/*******************************************************************************
 *
 * Create an instance of a macro controller
 *
 ******************************************************************************/
MacroController::MacroController(void)
	:PeriodicThread("macro controller", 0.05)
{
	macro_starts = 0;
	macro_completes = 0;
	macro_aborts = 0;

	macro_active_list.clear();
	macro_start_list.clear();
	macro_end_list.clear();

	macro_available_map.clear();

	new MacroStepProxy<MSWait>("gen","MSWait", this);
	new MacroStepProxy<MSExecute>("gen","MSExecute", this);
	new MacroStepProxy<MSSplit>("gen","MSSplit", this);
	new MacroStepProxy<MSJoin>("gen","MSJoin", this);
	new MacroStepProxy<MSOiDigitalState>("gen","MSOiDigitalState", this);
	new MacroStepProxy<MSIntSet>("gen","MSIntSet", this);
	new MacroStepProxy<MSFloatSet>("gen","MSFloatSet", this);
	new MacroStepProxy<MSForLoop>("gen","MSForLoop", this);
}

/*******************************************************************************
 *
 * Clean up all resources
 *
 * This includes stopping all active macros, then deleting all macros
 *
 ******************************************************************************/
MacroController::~MacroController(void)
{
	abortAll();

	map<string, Macro *>::iterator ittr = macro_available_map.begin();
	while(ittr != macro_available_map.end())
	{
		delete ittr->second;
		ittr++;
	}

	macro_available_map.clear();

	macro_active_list.clear();
	macro_start_list.clear();
	macro_end_list.clear();
}

/*******************************************************************************
 *
 * Load a macro file into a macro and put the resulting macro into the
 * controller
 *
 ******************************************************************************/
void MacroController::loadMacro(std::string name, std::string filename /*,
	RobotMain *robot_main*/)
{
	Macro *temp = new Macro(name /*, robot_main*/);
	temp->loadMacro(filename);
	putMacro(name, temp);
}

/*******************************************************************************
 *
 * Start a macro
 *
 * @param	name	the name of the macro to start
 *
 ******************************************************************************/
void MacroController::startMacro(Macro *macro)
{
	if (macro != nullptr)
	{
		macro_start_list.push_back(macro);
	}
}

/*******************************************************************************
 *
 * Start the named macro.  If the named macro is not in the list of 
 * available macros, nothing happens.
 * 
 ******************************************************************************/
void MacroController::startMacro(string macro_name)
{
	map<string, Macro *>::iterator rslt = macro_available_map.find(macro_name);
	if (rslt != macro_available_map.end())
	{
		startMacro(rslt->second);
	}
}

/*******************************************************************************
 *
 * Put the specified macro into the list of available macros, if there is 
 * already a macro in the list with that name, the existing macro will be
 * deleted and the new macro will be inserted in it's place.
 * 
 ******************************************************************************/
void MacroController::putMacro(string name, Macro *macro)
{
	map<string, Macro *>::iterator rslt = macro_available_map.find(name);
	if (rslt != macro_available_map.end())
	{
		Advisory::pwarning("MacroController::putMacro replacing macro %s", name.c_str());
		delete rslt->second;
	}

	macro_available_map[name] = macro;
	macro->setController(this);
}

/*******************************************************************************
 *
 * @return a pointer to the named macro, nullptr if there is not a macro with
 * 			the specified name.
 * 
 ******************************************************************************/
Macro * MacroController::getMacro(std::string name)
{
	map<string, Macro *>::iterator rslt = macro_available_map.find(name);
	if (rslt != macro_available_map.end())
	{
		return rslt->second;
	}

	return nullptr;
}

/*******************************************************************************
 *
 * Terminate a macro
 *
 * @param	name	the name of the macro to terminate
 *
 ******************************************************************************/
void MacroController::abortMacro(Macro *macro)
{
	vector<Macro *>::iterator ittr = std::find(macro_active_list.begin(),
			macro_active_list.end(), macro);

	if (ittr != macro_active_list.end())
	{
		Advisory::pinfo("MacroController::abortMacro ===============>  macro %s",
		    macro->getName().c_str());

		macro_aborts++;
		macro_completes--;
		macro->handleAbort();

		// don't do anything to remove the macro from the active list,
		// handleAbort will put the macro in a state that will cause
		// it to get removed within one cycle
	}
}

/*******************************************************************************
 *
 * Abort the named macro.  If the named macro is not in the list of
 * available macros, nothing happens.
 *
 ******************************************************************************/
void MacroController::abortMacro(string macro_name)
{
	map<string, Macro *>::iterator rslt = macro_available_map.find(macro_name);
	if (rslt != macro_available_map.end())
	{
		abortMacro(rslt->second);
	}
}

/*******************************************************************************
 *
 * Terminate all active macros
 *
 ******************************************************************************/
void MacroController::abortAll()
{
	for (Macro *macro : macro_active_list)
	{
		Advisory::pinfo("MacroController::abortAll ===============> aborting macro %s",
			    macro->getName().c_str());

		macro_aborts++;
		macro_completes--;
		macro->handleAbort();

		// don't do anything to remove the macro from the active list,
		// handleAbort will put the macro in a state that will cause
		// it to get removed within one cycle
	}
}

/*******************************************************************************
 *
 * If the name macros exists and is running, this method will return true.
 *
 ******************************************************************************/
bool MacroController::isRunning(std::string macro_name)
{
	Macro* macro = MacroController::getMacro(macro_name);

	if(macro == nullptr)
	{
		return false;
	}

	vector<Macro *>::iterator ittr = std::find(macro_active_list.begin(),
			macro_active_list.end(), macro);

	if (ittr == macro_active_list.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*******************************************************************************
 *
 * Update all active macros by calling the update method of all macros on
 * the active macro list.  Any macros that return as complete or are aborted
 * are removed from the active macro list. Any macros that are started are
 * added to the active list.
 *
 ******************************************************************************/
void MacroController::doPeriodic(void)
{
	//
	// Manage Active macros
	//
	for (Macro *macro : macro_active_list)
	{
		bool is_done = macro->update();
		if (is_done)
		{
			macro_end_list.push_back(macro);
		}
	}

	//
	// Manage any macros that have completed or aborted
	//
	for(Macro *macro : macro_end_list)
	{
		vector<Macro *>::iterator ittr = std::find(macro_active_list.begin(),
				macro_active_list.end(), macro);

		if (ittr != macro_active_list.end())
		{
			Advisory::pinfo("MacroController ===============> completing macro %s",
			    macro->getName().c_str());

			ittr = macro_active_list.erase(ittr);
			macro_completes++;
		}
	}
	macro_end_list.clear();

	//
	// Manage any macros that are starting
	//
	// Note: A macro that is starting may request another macro to start,
	// so copy and clear the start list before calling the init methods.
	// Any macros started in the initialization will need to wait until the
	// next cycle.
	//
	if (macro_start_list.size() > 0)
	{
		Advisory::pinfo("MacroController::update -- start list contains %d macros", macro_start_list.size());
		std::vector<Macro *>init_list;
		init_list.clear();
		for(Macro *macro : macro_start_list)
		{
			init_list.push_back(macro);
		}
		macro_start_list.clear();

		Advisory::pinfo("MacroController::update -- init list contains %d macros", init_list.size());
		for(Macro *macro : init_list)
		{
			// make sure this macro is not already in the active list, it's
			// possible that it got put on the start list more than once
			vector<Macro *>::iterator ittr = std::find(macro_active_list.begin(),
					macro_active_list.end(), macro);

			if (ittr == macro_active_list.end())
			{
				Advisory::pinfo("MacroController ===============> starting macro %s",
					macro->getName().c_str());
				macro_starts++;
				macro->init();
				macro_active_list.push_back(macro);
			}
		}

		init_list.clear();
	}
}

/*******************************************************************************
 *
 * Get the number of times that a macro was started
 *
 ******************************************************************************/
uint32_t MacroController::getMacroStarts(void)
{
	return macro_starts;
}

/*******************************************************************************
 *
 * Get the number of times that a macro completed successfully
 *
 ******************************************************************************/
uint32_t MacroController::getMacroCompletes(void)
{
	return macro_completes;
}

/*******************************************************************************
 *
 * Get the number of times that a macro aborted (either because of an
 * internal problem or because something external called requested the abort)
 *
 ******************************************************************************/
uint32_t MacroController::getMacroAborts(void)
{
	return macro_aborts;
}

/*******************************************************************************
 *
 * Get the number of macros that are currently running
 *
 ******************************************************************************/
uint32_t MacroController::getMacrosRunning(void)
{
	return macro_active_list.size();
}
