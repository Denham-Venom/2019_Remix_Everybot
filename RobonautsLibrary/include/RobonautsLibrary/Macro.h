/*******************************************************************************
 *
 * File: Macro.h
 * 
 * This file contains the definition of a class designed to hold a collection
 * of steps that get executed in sequence.
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

//#include "MacroStep.h"
class MacroStep; // need to forward declare to avoid circular reference
class RobotMain;
class MacroController;

#include <string>
#include <vector>
#include <map>

/*******************************************************************************
 *
 * This class is used to hold entry points into sequences of MacroSteps to
 * create the small chains or more complex trees and state diagrams that we
 * call Macros.  These chains can then be used to perform automated
 * sequences including the entire Auton opereation.
 *
 ******************************************************************************/
class Macro
{
	public:
		Macro(std::string name  /*, RobotMain *robot_main*/);
		MacroStep * setHead(MacroStep *step);
		MacroStep * setAbort(MacroStep *step);

		void init();
		bool update();
		void handleAbort();
		void abort();

		void loadMacro(std::string macro_file);

//		RobotMain * getRobotMain();
		std::string getName();
		std::string getDescription();

		void setIntVar(std::string name, int val);
		int  getIntVar(std::string name);
		
		void setFloatVar(std::string name, float val);
		float  getFloatVar(std::string name);

		void setController(MacroController *controller);

		bool isRunning();
		bool lastExecAborted();

	private:
		typedef std::map<std::string, MacroStep*> MacroStepMap;

		void startNextStep(MacroStep *next);

		std::map<std::string, int> var_int;
		std::map<std::string, float> var_float;
		std::map<std::string, MacroStep*> macro_steps;
		
		std::string macro_name;
		std::string macro_description;

//		RobotMain *robot_main;
		MacroController *macro_controller;

		MacroStep *head_step;
		MacroStep *abort_step;
		MacroStep *current_step;

		bool last_exec_aborted;
};
