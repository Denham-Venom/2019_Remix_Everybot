/*******************************************************************************
 *
 * File: MacroStep.cpp
 *
 * This file contains the definition of several base classes for MacroSteps
 * as well as some generic MacroStep classes.
 *
 *  - MacroStep
 *  - MacroStepProxy
 *
 *  - MacroStepSequence
 *  - MacroStepCondition
 *  - MacroStepCompare
 *
 *  - MSWait
 *  - MSSplit
 *  - MSJoin
 *  - MSForLoop
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
#include "RobonautsLibrary/MacroStep.h"
#include "RobonautsLibrary/MacroStepFactory.h"
#include "RobonautsLibrary/OIController.h"

#include "gsu/Advisory.h"

#ifdef _2018
#include "Timer.h"
#else
#include "frc/Timer.h"
#endif

using namespace frc;


// =============================================================================
// === MacroStep Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MacroStep::MacroStep(std::string type, tinyxml2::XMLElement *xml, void *control)
{
	parent_macro = NULL;
	step_type = type;
	step_name = "unknown";
	is_clear = false;

	step_control = control;
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep::~MacroStep()
{
	parent_macro = NULL;
	step_type = "unknown";
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStep::abort()
{
	if (parent_macro != NULL)
	{
		parent_macro->abort();
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStep::setParentMacro(Macro *macro)
{
	if (macro == NULL)
	{
		Advisory::pwarning("%s setting parent macro to NULL",
		    this->toString().c_str());
		Advisory::pinfo(
		    "steps should only be chained after the head is added to a macro");
	}

	parent_macro = macro;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStep::setUnclear(void)
{
	is_clear = false;
}
void MacroStep::setName(std::string name)
{
	step_name = name;
}
/*******************************************************************************
 *
 ******************************************************************************/
std::string MacroStep::toString()
{
	std::string rtn = "MacroStep:";
	rtn.append(step_type);
	rtn.append("(");
	rtn.append(step_name);
	rtn.append(")");
	return (rtn);
}

// =============================================================================
// === MacroStepSequence Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MacroStepSequence::MacroStepSequence(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStep(type, xml, control)
{
	next_step = NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStepSequence::~MacroStepSequence()
{
	if (next_step != NULL)
	{
		delete next_step;
		next_step = NULL;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MacroStepSequence::connect(std::string which, MacroStep * step)
{
	if (which.compare("next") == 0)
	{
		next_step = step;
		if (next_step != NULL)
		{
			next_step->setParentMacro(parent_macro);
		}

		return next_step;
	}

	Advisory::pinfo(
	    "MacroStepSequence::connect invalid connection of %s for %s ignored",
	    which.c_str(), step_type.c_str());

	return NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStepSequence::clear(void)
{
	if (! is_clear)
	{
		is_clear = true; // set is_clear before calling connections to 
		            	 // stop a recursive loop
		if (next_step != NULL)
		{
			next_step->clear();
		}
	}
}

// =============================================================================
// === MacroStepCondition Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MacroStepCondition::MacroStepCondition(std::string type, 
	tinyxml2::XMLElement *xml, void *control) : MacroStep(type, xml,control)
{
	true_step = NULL;
	false_step = NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStepCondition::~MacroStepCondition()
{
	if (true_step != NULL)
	{
		delete true_step;
		true_step = NULL;
	}

	if (false_step != NULL)
	{
		delete false_step;
		false_step = NULL;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MacroStepCondition::connect(std::string which, MacroStep * step)
{
	if (which.compare("true") == 0)
	{
		true_step = step;
		if (true_step != NULL)
		{
			true_step->setParentMacro(parent_macro);
		}
		return true_step;
	}
	else if (which.compare("false") == 0)
	{
		false_step = step;
		if (false_step != NULL)
		{
			false_step->setParentMacro(parent_macro);
		}
		return false_step;
	}

	Advisory::pinfo(
	    "MacroStepCondition::connect invalid connection of %s for %s ignored",
	    which.c_str(), step_type.c_str());

	return NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStepCondition::clear(void)
{
	if (! is_clear)
	{
		is_clear = true; // set is_clear before calling connections to 
		            	 // stop a recursive loop

		if (true_step != NULL)
		{
			true_step->clear();
		}
		
		if (false_step != NULL)
		{
			false_step->clear();
		}
	}
}

// =============================================================================
// === MacroStepCompare Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MacroStepCompare::MacroStepCompare(std::string type, tinyxml2::XMLElement *xml, void *control):
	MacroStep(type, xml, control)
{
	gt_step = NULL;
	eq_step = NULL;
	lt_step = NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStepCompare::~MacroStepCompare()
{
	if (gt_step != NULL)
	{
		delete gt_step;
		gt_step = NULL;
	}

	if (eq_step != NULL)
	{
		delete eq_step;
		eq_step = NULL;
	}

	if (lt_step != NULL)
	{
		delete lt_step;
		lt_step = NULL;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MacroStepCompare::connect(std::string which, MacroStep * step)
{
	if (which.compare("gt") == 0)
	{
		gt_step = step;
		if (gt_step != NULL)
		{
			gt_step->setParentMacro(parent_macro);
		}
		return gt_step;
	}
	else if (which.compare("eq") == 0)
	{
		eq_step = step;
		if (eq_step != NULL)
		{
			eq_step->setParentMacro(parent_macro);
		}
		return eq_step;
	}
	else if (which.compare("lt") == 0)
	{
		lt_step = step;
		if (lt_step != NULL)
		{
			lt_step->setParentMacro(parent_macro);
		}
		return lt_step;
	}

	Advisory::pinfo(
	    "MacroStepCompare::connect invalid connection of %s for %s ignored",
	    which.c_str(), step_type.c_str());

	return NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStepCompare::clear(void)
{
	if (! is_clear)
	{
		is_clear = true; // set is_clear before calling connections to 
		            	 // stop a recursive loop
		
		if (gt_step != NULL)
		{
			gt_step->clear();
		}
		if (eq_step != NULL)
		{
			eq_step->clear();
		}
		if (lt_step != NULL)
		{
			lt_step->clear();
		}
	}
}

// =============================================================================
// === MacroStepCompareGameData Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MacroStepCompareGameData::MacroStepCompareGameData(std::string type, tinyxml2::XMLElement *xml, void *control):
	MacroStepCompare(type, xml, control)
{
	//TBD
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStepCompareGameData::~MacroStepCompareGameData()
{
	if (gt_step != NULL)
	{
		delete gt_step;
		gt_step = NULL;
	}

	if (eq_step != NULL)
	{
		delete eq_step;
		eq_step = NULL;
	}

	if (lt_step != NULL)
	{
		delete lt_step;
		lt_step = NULL;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MacroStepCompareGameData::connect(std::string which, MacroStep * step)
{
	return NULL;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MacroStepCompareGameData::clear(void)
{
}

// =============================================================================
// === MSWait Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MSWait::MSWait(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepSequence(type, xml, control)
{
	wait_time = xml->FloatAttribute("time");
	expire_time = 0.0;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSWait::init()
{
	expire_time = GetTime() + wait_time;
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSWait::update()
{
	if (GetTime() > expire_time)
	{
		return next_step;
	}
	return this;
}

// =============================================================================
// === MSSplit Methods
// =============================================================================
/*******************************************************************************
 *@run_split: arg 0 runs macro one
 ******************************************************************************/
MSSplit::MSSplit(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepSequence(type, xml, control)
{
	run_macro_one = xml->Attribute("macro_name");
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSSplit::init()
{
	((MacroController *)step_control)->startMacro(run_macro_one);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSSplit::update()
{
	return next_step;
}

// =============================================================================
// === MSJoin Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MSJoin::MSJoin(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepSequence(type, xml, control)
{
	m_macro_controller = (MacroController*)control;
	m_macro_name = xml->Attribute("macro_name");

	m_timeout_time = 0.0;
	m_timeout_duration = 600.0;
	m_on_timeout_abort = false;

	xml->QueryDoubleAttribute("timeout", &m_timeout_duration);

	xml->QueryBoolAttribute("abort_on_timeout", &m_on_timeout_abort);

	const char* on_timeout_string = xml->Attribute("on_timeout");
	if (on_timeout_string != nullptr)
	{
		if (strcmp(on_timeout_string, "abort") == 0)
		{
			m_on_timeout_abort = true;
		}
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSJoin::init()
{
	m_timeout_time = GetTime() + m_timeout_duration;
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSJoin::update()
{

	if(GetTime() > m_timeout_time)
	{
		if(m_on_timeout_abort)
		{
			parent_macro->abort();
			return nullptr;
		}
		else
		{
			return next_step;
		}
	}

	if(!(m_macro_controller->isRunning(m_macro_name)))
	{
		return next_step;
	}

	return this;

}

// =============================================================================
// === MSExecute Methods
// =============================================================================
/*******************************************************************************
	<step type="Execute" ctrl_name="gen" name="do_other_thing" macro_name="other_thing" abort_action="abort" >
		<connect type="next" step="drive_long"/>
	</step>
 ******************************************************************************/
MSExecute::MSExecute(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepSequence(type, xml, control)
{
	macro_to_execute = nullptr;
	macro_name_to_execute = xml->Attribute("macro_name");
	abort_on_abort = true;
	const char *abort_action = xml->Attribute("abort_action");
	if ((nullptr != abort_action) && (strcmp(abort_action, "abort") != 0))
	{
		abort_on_abort = false;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSExecute::init()
{
	macro_to_execute = ((MacroController *)step_control)->getMacro(macro_name_to_execute);
	if (nullptr == macro_to_execute)
	{
		Advisory::pwarning("MSExecute could not start macro %s", macro_name_to_execute.c_str());
	}
	else
	{
		((MacroController *)step_control)->startMacro(macro_to_execute);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSExecute::update()
{
	if (nullptr == macro_to_execute)
	{
		if (true == abort_on_abort)
		{
			abort();
			return nullptr;
		}

		return next_step;
	}

	if (true != macro_to_execute->isRunning())
	{
		if ((true == abort_on_abort) && (true == macro_to_execute->lastExecAborted()))
		{
			abort();
			return nullptr;
		}

		return next_step;
	}

	return this;
}


// =============================================================================
// === MSOiDigitalState Methods
// =============================================================================
/*******************************************************************************
 *
 ******************************************************************************/
MSOiDigitalState::MSOiDigitalState(std::string type, tinyxml2::XMLElement *xml, void *control)
	: MacroStepCondition(type, xml, control)
{
	m_oi_state = true;
	xml->QueryBoolAttribute("default_state", &m_oi_state);
	OIController::subscribeDigital(xml, this, 0);
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSOiDigitalState::init(void)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep * MSOiDigitalState::update(void)
{
	if (m_oi_state)
	{
		return true_step;
	}
	else
	{
		return false_step;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSOiDigitalState::setDigital(int oi_id, bool oi_digital_state)
{
	m_oi_state = oi_digital_state;
}

// =============================================================================
// === MSIntSet Methods
// =============================================================================
/*******************************************************************************
 *
 *@param arg[0] = name
 *@param arg[1] = value
 *
 ******************************************************************************/
MSIntSet::MSIntSet(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepSequence(type, xml, control)
{
	var_name = xml->Attribute("var_name");
	
	try
	{
		var_value = xml->IntAttribute("var_value");
	}
	catch (...)
	{
		var_value = 0;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSIntSet::init()
{
	parent_macro->setIntVar(var_name, var_value);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSIntSet::update()
{
	return next_step;
}

// =============================================================================
// === MSFloatSet Methods
// =============================================================================
/*******************************************************************************
 *
 *@param arg[0] = name
 *@param arg[1] = value
 *
 ******************************************************************************/
MSFloatSet::MSFloatSet(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepSequence(type, xml, control)
{
	var_name = xml->Attribute("var_name");

	try
	{
		var_value = xml->FloatAttribute("var_value");
	}
	catch (...)
	{
		var_value = 0;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSFloatSet::init()
{
	parent_macro->setFloatVar(var_name, var_value);
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSFloatSet::update()
{
	return next_step;
}

// =============================================================================
// === MSForLoop Methods
// =============================================================================
/*******************************************************************************
 *@run_for_loop: arg 0 num_loop_cycles
 ******************************************************************************/
MSForLoop::MSForLoop(std::string type, tinyxml2::XMLElement *xml, void *control) :
	MacroStepCondition(type, xml, control)
{
	try
	{
		num_loop_cycles = xml->IntAttribute("cycles");
	}
	catch (...)
	{
		num_loop_cycles = 0;
	}

	cur_loop_cycle = -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSForLoop::init()
{
	cur_loop_cycle++;
}

/*******************************************************************************
 *
 ******************************************************************************/
void MSForLoop::clear(void)
{
	cur_loop_cycle = -1;
	MacroStepCondition::clear();
}

/*******************************************************************************
 *
 ******************************************************************************/
MacroStep *MSForLoop::update()
{
	if (cur_loop_cycle < num_loop_cycles)
	{
		return true_step;
	}

	cur_loop_cycle = -1;
	return false_step;
}
