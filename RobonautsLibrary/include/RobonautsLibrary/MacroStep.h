/*******************************************************************************
 *
 * File: MacroStep.h
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
#pragma once

#include <map>
#include <string>
#include <vector>

#include "gsu/tinyxml2.h"

#include "RobonautsLibrary/Macro.h"
#include "RobonautsLibrary/MacroController.h"
#include "RobonautsLibrary/OIObserver.h"

using namespace tinyxml2;

/*******************************************************************************
 *
 * The MacroStep base classes provide the structure for linking the steps
 * of a macro together.  Subclasses of these macro steps exist for each small
 * capability of the robot.  Instances of those classes are linked together
 * into small chains or more complex trees and state diagrams to create
 * what we call Macros.  These chains can then be used to perform automated
 * sequences including the entire Auton opereation.
 *
 ******************************************************************************/
class MacroStep
{
	public:
		MacroStep(std::string type, tinyxml2::XMLElement *xml, void *control);
		virtual ~MacroStep();

		virtual MacroStep * connect(std::string which, MacroStep * step) = 0;
		
		void setUnclear(void); // should be called by Macro
		
		virtual void clear(void) = 0;
		virtual void init(void) = 0;
		virtual MacroStep * update(void) = 0;
		virtual std::string toString();
		virtual void setName(std::string name);
		void abort();
		void setParentMacro(Macro *macro);

	protected:
		Macro *parent_macro;
		std::string step_type;
		bool is_clear;
		std::string step_name;
		void *step_control;
};

/*******************************************************************************
 *
 * The Sequence step (and any class the extends this class) only have one exit
 * condition,  when finished they always advane to the "next" step.
 * 
 ******************************************************************************/
class MacroStepSequence : public MacroStep
{
	public:
		MacroStepSequence(std::string type, tinyxml2::XMLElement *xml, void *control);
		virtual ~MacroStepSequence();

		MacroStep * connect(std::string which, MacroStep * step);

		virtual void clear(void);

	protected:
		MacroStep * next_step;
};

/*******************************************************************************
 *
 * The Condition step (and any class that extends this class) has two exit
 * conditions.  Something inside of the update method must determine if the
 * next step should be the "true" step or the "false" step.  For example,
 * if digital input 3 is high, return the true step else return the false step.
 * 
 ******************************************************************************/
class MacroStepCondition : public MacroStep
{
	public:
		MacroStepCondition(std::string type, tinyxml2::XMLElement *xml, void *control);
		virtual ~MacroStepCondition();

		MacroStep * connect(std::string name, MacroStep * step);

		virtual void clear(void);

	protected:
		MacroStep * true_step;
		MacroStep * false_step;
};

/*******************************************************************************
 *
 * The Compare step (and any class that extends this class) has three exit
 * conditions.  Something in the update method must determine if the
 * "gt" (greater than), "eq" (equal), or "lt" (less than) step is returned.
 * 
 ******************************************************************************/
class MacroStepCompare : public MacroStep
{
	public:
		MacroStepCompare(std::string type, tinyxml2::XMLElement *xml, void *control);
		virtual ~MacroStepCompare();

		MacroStep * connect(std::string which, MacroStep * step);

		virtual void clear(void);

	protected:
		MacroStep * gt_step;
		MacroStep * eq_step;
		MacroStep * lt_step;
};


/*******************************************************************************
 *
 * The GameData step (and any class that extends this class) inherits from 
 * compare and has string condition compares as well as data from control
 * 
 ******************************************************************************/
class MacroStepCompareGameData : public MacroStepCompare
{
	public:
		MacroStepCompareGameData(std::string type, tinyxml2::XMLElement *xml, void *control);
		virtual ~MacroStepCompareGameData();
		MacroStep * connect(std::string which, MacroStep * step);

		virtual void clear(void);

	protected:
                // TBD
};

/*******************************************************************************
 *
 * The MSWait step is a sequence step that will return the next step after 
 * the specified amount of time passes
 * 
 ******************************************************************************/
class MSWait : public MacroStepSequence
{
	public:
		MSWait(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		double wait_time;
		double expire_time;
};

/*******************************************************************************
 *
 * The MSExecute class will start another macro then wait for it to complete
 * before advancing to the next step.  If the executed macro aborts this
 * step will can either abort this macro, or continue with execution
 *
 ******************************************************************************/
class MSExecute : public MacroStepSequence
{
	public:
		MSExecute(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		std::string macro_name_to_execute;
		Macro *macro_to_execute;
		bool abort_on_abort;
};

/*******************************************************************************
 *
 * The MSSplit class will start another macro then return the next step.  This
 * allows for multiple concurrent activities, for example the robot could
 * drive through a sequence of turns while preparing the arm and claw for
 * the whatever happens when the robot arrives at some location
 * 
 ******************************************************************************/
class MSSplit : public MacroStepSequence
{
	public:
		MSSplit(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		std::string run_macro_one;

};

/*******************************************************************************
 *
 * The MSJoin class will
 ******************************************************************************/
class MSJoin : public MacroStepSequence
{
	public:
		MSJoin(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

	private:
		std::string m_macro_name; //The name of the macro we're waiting for
		MacroController* m_macro_controller;
		double m_timeout_time;
		double m_timeout_duration;
		bool m_on_timeout_abort;
};

/*******************************************************************************
 *
 * This Macro Step subscribes to an oi button/switch then will allow the
 * macro to make a decision based on it's the oi state.
 *
 *  Example XML:
 *
 * <step type="MSOiDigitalState" ctrl_name="gen" name="is_red_side"
 * 		device="switches" chan="7" invert="false" default_state="true" >
 *
 *      <connect type="true" step="do_red_stuff"/>
 *      <connect type="false" step="do_blue_stuff"/>
 * </step>
 *
 ******************************************************************************/
class MSOiDigitalState : public MacroStepCondition, public OIObserver
{
	public:
		MSOiDigitalState(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep * update(void);

		void setDigital(int id, bool val);

	private:
		bool m_oi_state;
};

/*******************************************************************************
 *
 * The MSIntSet class will store the specified value with the specified name
 * for use in other steps
 * 
 ******************************************************************************/
class MSIntSet : public MacroStepSequence
{
	public:
		MSIntSet(std::string type, tinyxml2::XMLElement *xml, void *control);
		
		void init(void);
		MacroStep  * update(void);
		
	private:
		std::string var_name;
		int	var_value;
};

/*******************************************************************************
 *
 * The MSFloatSet class will store the specified value with the specified name
 * for use in other steps
 *
 ******************************************************************************/
class MSFloatSet : public MacroStepSequence
{
	public:
		MSFloatSet(std::string type, tinyxml2::XMLElement *xml, void *control);

		void init(void);
		MacroStep  * update(void);

	private:
		std::string var_name;
		float var_value;
};

// class MSIntIncrement : public MacroStepSequence
// class MSIntDecrement : public MacroStepSequence
// class MSIntCompare : public MacroStepSequence

/*******************************************************************************
 *
 * @TODO: delete this class, use MSIntForLoop instead
 * 
 * The MSForLoop allows for loops to be programmed into the macro by keeping
 * a count of how many times the step is entered, returning the "true" step
 * as long as the content of the loop should be executed and the "false" setp
 * when the loop should end.
 * 
 * WARNING: at this time this does not work well if the loop is interupted,
 * as there is no way to reset the loop count, so the next time the macro
 * runs, it will continue with the previous runs count
 * 
 ******************************************************************************/
class MSForLoop : public MacroStepCondition
{
	public:
		MSForLoop(std::string type, tinyxml2::XMLElement *xml, void *control);
		
		virtual void clear(void);
		void init(void);
		MacroStep * update(void);

	private:
		int num_loop_cycles;
		int cur_loop_cycle;
};
