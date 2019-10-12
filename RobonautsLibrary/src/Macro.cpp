/*******************************************************************************
 *
 * File: Macro.cpp
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

#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "RobonautsLibrary/DataLogger.h"
#include "RobonautsLibrary/Macro.h"
#include "RobonautsLibrary/MacroStep.h"
#include "RobonautsLibrary/MacroStepFactory.h"
#include "RobonautsLibrary/RobotUtil.h"
#include "gsu/tinyxml2.h"
#include "gsu/Advisory.h"

#ifdef _2018
#include "Timer.h"
#else
#include "frc/Timer.h"
#endif

using namespace tinyxml2;
using namespace std;
using namespace frc;

/*******************************************************************************
 *
 * Create a macro and initialize all members to something safe
 *
 * @param	name	the name of this macro
 *
 ******************************************************************************/
Macro::Macro(string name /*, RobotMain *robot*/)
{
	macro_name = name;
	macro_description = name;
//	robot_main = robot;

	macro_controller = nullptr;
	head_step = nullptr;
	abort_step = nullptr;
	current_step = nullptr;

	last_exec_aborted = false;
}

/*******************************************************************************
 *
 * Set the first step of this macro.  This sets the parent macro of the
 * provided macro step to this macro, additional steps should not be added
 * to the provided step until after it is added to a macro.  This order of
 * doing things allow the adding of subsequent steps to be associated with
 * this macro 'automatically'.
 *
 * @param	step	a pointer to the first Macro step that should be executed
 *                  when this macro is run.
 *
 * @return	a pointer to the provided step is returned so additional steps
 *          can be added in place after it.
 *
 ******************************************************************************/
MacroStep * Macro::setHead(MacroStep *step)
{
	head_step = step;
	head_step->setParentMacro(this);
	return head_step;
}

/*******************************************************************************
 *
 * Set the first abort step of this macro.  This sets the parent macro of the
 * provided macro step to this macro, additional steps should not be added
 * to the provided step until after it is added to a macro.  This order of
 * doing things allow the adding of subsequent steps to be associated with
 * this macro 'automatically'.
 *
 * NOTE: Any steps being added in the abort sequence should return NULL, or
 * 		the next abort step the first time the update method is called so
 * 		the abort sequence can be completed in a single cycle.
 *
 * @param	step	a pointer to the first Macro step that should be executed
 *                  when this macro is run.
 *
 * @return	a pointer to the provided step is returned so additional steps
 *          can be added in place after it.
 *
 ******************************************************************************/
MacroStep * Macro::setAbort(MacroStep *step)
{
	abort_step = step;
	abort_step->setParentMacro(this);
	return abort_step;
}

/*******************************************************************************
 *
 ******************************************************************************/
//RobotMain * Macro::getRobotMain(void)
//{
//	return robot_main;
//}

/*******************************************************************************
 *
 ******************************************************************************/
void Macro::setController(MacroController *controller)
{
	macro_controller = controller;
}

/*******************************************************************************
 *
 * @return true if this macro is running
 *
 ******************************************************************************/
bool Macro::isRunning()
{
	return (current_step != NULL);
}

/*******************************************************************************
 *
 * Start running this macro
 *
 ******************************************************************************/
void Macro::init()
{
	last_exec_aborted = false;

	if (head_step != NULL)
	{
		head_step->clear();
	}
	
	if (abort_step != NULL)
	{
		abort_step ->clear();
	}
	
	startNextStep(head_step);
}

/*******************************************************************************
 *
 * Terminate this macro
 *
 ******************************************************************************/
void Macro::handleAbort()
{
	if (current_step != nullptr)
	{
		Advisory::pinfo("%8.3f  Macro::%s - aborting %s", GetTime(),
		    macro_name.c_str(), current_step->toString().c_str());
	}

	startNextStep(abort_step);

	// any steps in the abort chain should return the next step or NULL on the
	// first call to update so the chain can be traversed quickly.
	int cnt = 0;
	while ((current_step != nullptr) && (cnt++ < 20))
	{
		MacroStep * next_abort = current_step->update();

		if (next_abort == current_step)
		{
			Advisory::pinfo(
			    "%8.3f  Macro::%s - abort not completed, invalid abort step %s",
			    GetTime(), macro_name.c_str(), current_step->toString().c_str());

			current_step = nullptr;
		}
		else
		{
			startNextStep(next_abort);
		}
	}
	current_step = nullptr;
	last_exec_aborted = true;
}

/*******************************************************************************
 *
 * Terminate this macro
 *
 ******************************************************************************/
void Macro::abort()
{
	macro_controller->abortMacro(this);
}

/*******************************************************************************
 *
 * Update this macro by updating the current step and checking to see if
 * that step is finished.  If it is finished, start the next step.
 *
 * @return	true if this macro is complete.  That is if the last step in
 * 				one of this macros chains has finished.
 *
 ******************************************************************************/
bool Macro::update()
{
	if (current_step == nullptr)
	{
		return true;
	}

	MacroStep *next = current_step->update();
	if (next != current_step)
	{
		startNextStep(next);
	}

	return (current_step == nullptr);
}

/*******************************************************************************
 *
 * @return the name of this macro
 *
 ******************************************************************************/
string Macro::getName()
{
	return macro_name;
}

/*******************************************************************************
 *
 * @return the name of this macro
 *
 ******************************************************************************/
string Macro::getDescription()
{
	return macro_description;
}

/*******************************************************************************
 *
 * @return true if the last execution of this macro aborted
 *
 ******************************************************************************/
bool Macro::lastExecAborted()
{
	return last_exec_aborted;
}

/*******************************************************************************
 *
 * This private method is used to set the current step to the provided
 * argument and initialize the newly appointed current step
 *
 * @param	next	the step that should be set as the current step
 *
 ******************************************************************************/
void Macro::startNextStep(MacroStep *next)
{
	current_step = next;

	if (current_step != nullptr)
	{
		Advisory::pinfo("%8.3f  Macro::%s - starting %s", GetTime(),
		    macro_name.c_str(), current_step->toString().c_str());
		current_step->setUnclear();
		current_step->init();
	}
}

/*******************************************************************************
 *
 * Set an integer variable with visibility to any step in this macro.
 *
 * @param	name	the name of the variable
 * @param	val		the new value for the parameter
 *
 ******************************************************************************/
void Macro::setIntVar(std::string name, int val)
{
	var_int[name] = val;
}

/*******************************************************************************
 *
 * Get an integer variable with visibility to any step in this macro.
 *
 * @param	name	the name of the variable
 *
 * @return the current value for the parameter
 *
 ******************************************************************************/
int  Macro::getIntVar(std::string name)
{
	std::map<std::string, int>::iterator ittr = var_int.find(name);
	if (ittr == var_int.end())
	{
		return 0;
	}
	
	return ittr->second;
}

/*******************************************************************************
 *
 * Set an integer variable with visibility to any step in this macro.
 *
 * @param	name	the name of the variable
 * @param	val		the new value for the parameter
 *
 ******************************************************************************/
void Macro::setFloatVar(std::string name, float val)
{
	var_float[name] = val;
}

/*******************************************************************************
 *
 * Get an integer variable with visibility to any step in this macro.
 *
 * @param	name	the name of the variable
 *
 * @return the current value for the parameter
 *
 ******************************************************************************/
float  Macro::getFloatVar(std::string name)
{
	std::map<std::string, float>::iterator ittr = var_float.find(name);
	if (ittr == var_float.end())
	{
		return 0;
	}

	return ittr->second;
}


/*******************************************************************************
 *
 * Create this macro by read the definition from a file.
 *
 * The file can contain two types of directives -- create and connect.  It can
 * also contain blank lines and  comments.  Any line that startes with # will
 * be considered a comment.
 *
 *   create <step_type> <step_name> [step_args_0] [step_arg_1] [...] [step_arg_n]
 *   connect <src_step_name> <connect_type> <dest_step_name>
 *
 *   step_type - specifies the name used to register a MacroStep class with the
 *   			 factory.
 *
 *   step_name, src_step_name, and dest_step_name - a unique string given to each
 *   	 step so multiple instances of a type can be used.
 *
 *   step_arg[s] - a variable number of arguments that get passed to the MacroStep
 *                 as it is being created.
 *
 *   connect_type - is specific to the source type
 *        MacroStepSequence supports "next"
 *        MacroStepCondition supports "true" and "false"
 *        MacroStepCompare supports "lt" (less than), "eq" (equal), and "gt" (greater than)
 *
 *   	  There are special connects with the src_step_name of "macro" the
 *   	  connect_type of "head" can be used to inidicate the first step in
 *   	  the macro and the connect type of "abort" can be used to indicate
 *   	  the first step to be performed if this macro aborts.
 *
 * NOTE: a step must be created before it can be used in a connect statement.
 *
 * @param	macro_file	the fully qualified file name to be read
 *
 ******************************************************************************/
void Macro::loadMacro(std::string macro_file)
{
	XMLDocument doc;
	XMLError err;
	err = doc.LoadFile(macro_file.c_str());

	if(err != XML_SUCCESS)
	{
		Advisory::pwarning("could not read macro file %s", macro_file.c_str());
		return;
	}

	XMLElement *macro = doc.FirstChildElement("macro");
	const char *ctrl;
	const char *type;
	const char *name;
	

	if (macro == nullptr)
	{
		Advisory::pwarning("could not read macro config file %s, no \"macro\" element",
			macro_file.c_str());
		return;
	}
	
	macro_description = macro->Attribute("name");
	if (!macro_description.empty())
	{
		Advisory::pinfo("Macro  Loading : %s", macro_description.c_str());
	}
	else
	{
		macro_description = "unknown";
		Advisory::pinfo("Macro  Loading : unnamed");
	}
		
	XMLElement *step_xml = macro->FirstChildElement("step");
	while (step_xml != NULL)
	{
		ctrl = step_xml->Attribute("ctrl_name");
		if (ctrl == nullptr)
		{
			ctrl = step_xml->Attribute("control");
		}

		type = step_xml->Attribute("type");
		name = step_xml->Attribute("name");
		
//		Advisory::pinfo("Creating step type %s name %s", type, name);

		if ((name != nullptr) && (type != nullptr) && (ctrl != nullptr))
		{
//			Advisory::pinfo("%s: %d", __FILE__, __LINE__);
			MacroStep *mstep = MacroStepFactory::create(ctrl, type, step_xml);
			if (mstep != nullptr)
			{
//				Advisory::pinfo("%s: %d", __FILE__, __LINE__);
				mstep->setName(name);
				if (mstep != NULL)
				{
					Advisory::pinfo("  created step: %s %s - %s ", ctrl, type, name);
					macro_steps.insert(std::pair<std::string, MacroStep*>(name, mstep));
				}
				else
				{
					Advisory::pwarning("failed to create control %s %s - %s", ctrl, type, name);
				}
			}
			else
			{
				Advisory::pwarning("  factory could not create step: %s %s - %s ", ctrl, type, name);
			}
		}
		else
		{
			Advisory::pwarning("could not create macro step -- control, type or name is NULL");
		}

		step_xml = step_xml->NextSiblingElement("step");
	}
		
	XMLElement *elem = macro->FirstChildElement("connect");
	
	while (elem != NULL)
	{
		type = elem->Attribute("type");
		name = elem->Attribute("step");
		
		if (strcmp(type, "head") == 0)
		{
			MacroStepMap::iterator conn_step = macro_steps.find(name);
			if (conn_step != macro_steps.end())
			{
				Advisory::pinfo("  connecting macro head to %s", name);
				setHead(conn_step->second);
			}
			else
			{
				Advisory::pwarning("could not connect macro head to unknown node %s", name);
			}
		}
		else if (strcmp(type, "abort") == 0)
		{
			MacroStepMap::iterator conn_step = macro_steps.find(name);
			if (conn_step != macro_steps.end())
			{
				Advisory::pinfo("  connecting macro abort to %s", name);
				setAbort(conn_step->second);
			}
			else
			{
				Advisory::pwarning("could not connect macro abort to unknown node %s", name);
			}
		}
		else
		{
			Advisory::pwarning("  unknown macro connection type");
		}

		elem = elem->NextSiblingElement("connect");
	}
//	else
//	{
//		Advisory::pinfo("ERROR: no macro connection to head step");
//	}
	
	step_xml = macro->FirstChildElement("step");
	while (step_xml != nullptr)
	{
		const char *src_name = step_xml->Attribute("name");
		Advisory::pinfo("  connecting step %s", src_name);

		MacroStepMap::iterator src_step = macro_steps.find(src_name);
		if (src_step != macro_steps.end())
		{
			XMLElement *conn_xml = step_xml->FirstChildElement("connect");

			while(conn_xml != nullptr)
			{
				const char *dest_name = conn_xml->Attribute("step");
				MacroStepMap::iterator dest_step = macro_steps.find(dest_name);
				
				if (dest_step != macro_steps.end())
				{
					type = conn_xml->Attribute("type");
					
					Advisory::pinfo("    connecting  %s - %s --> %s", src_name, type, dest_name);
					src_step->second->connect(type, dest_step->second);
				}
				else
				{
					Advisory::pwarning("connection cannot be made from %s to unknown destination name of %s",
						src_name, dest_name);
				}
				conn_xml = conn_xml->NextSiblingElement("connect");
			}
		}
		else
		{
			Advisory::pwarning("Macro::loadMacro(%s): did not find node for connections -- %s", macro_file.c_str(), src_name);
		}
		
		step_xml = step_xml->NextSiblingElement("step");
	}	
}

