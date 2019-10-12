/*******************************************************************************
 *
 * File: MacroStepFactor.h
 * 
 * This file contains the definition of the MacroStepFactory class, the 
 * MacroStepProxy base class, a pre-processor macro for defining and 
 * instanttiating proxies, and an explanation of how the MacroStep, 
 * MacroStepProxy, Macro, MacroController, and MacroStepFactory work together.
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
#include <string>

#include "gsu/tinyxml2.h"

#include "RobonautsLibrary/MacroStep.h"

/*******************************************************************************
 *******************************************************************************

 A Macro is a collection of commands that get executed in a sequence, they can
 be visualized as flow charts.

 A MacroStep is a single command, action, or check in the operation of the
 robot.  A MacroStep would be one block on the flowchart.

 Each functional block of the robot can extend the MacroStep base
 classes (MacroStep, MacroStepSequence, MacroStepCondition, or MacroStepCompare
 to provide Macro access to components of that functional block. For example,
 the DriveSystem could define a MacroStep that drives straight for some
 distance.

 When a MacroStep is defined, a MacroStepProxy must also be defined. A
 single static instance of each proxy must be created.  The MSF_REGISTER 
 pre-processor macro defined below can be used to define and instantiate the 
 needed proxy. The creation of this proxy causes it to be registered with the 
 MacroStepFactory.  Thus the factory does not need to know about all of the 
 MacroSteps when the factory is compiled and the factory code does not need 
 to change when new MacroSteps are created.  The list of available steps 
 isn't populated until the robot code starts to run.  This is sometimes 
 referred to as self registering objects.

 So, after the robot code starts, the MacroStepFactory will hold a list of all
 of the available MacroSteps and will be able to create any of them via the
 matching MacroStepProxy.

 Now Macros can be defined in relatively simple text files (or in code) to
 execute various commands on the robot.  The macros are added to the
 MacroController so at any given time zero, one, or more macros can be
 running and the main robot code doesn't need to manage them.  The main
 robot code just gives the MacroController a chance to work by calling the
 update method each period.

 Any trigger in the software can be used to start a Macro, a button press,
 a sensor, or even the start of autonomous operations.

 *******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *
 * The MacroStepProxy class is the base class for all proxy classes.  These
 * proxy classes are used to register a MacroStep class with the factory so
 * instances can be created via the factory.
 * 
 * Most MacroStep classes will be able to use the c preprocessor macro defined
 * below to declare and create a single proxy instance.  Howerver, if the
 * MacroStep requires special construction a separate proxy that extends this
 * base class can be declared and instantiated.
 * 
 ******************************************************************************/
class MacroStepProxyBase
{
	public:
		MacroStepProxyBase(void *control)
		{
			mspb_control = control;
		}
		
		virtual ~MacroStepProxyBase()
		{
		}

		virtual MacroStep * create(std::string type, tinyxml2::XMLElement *xml) = 0;

	protected:
		void *mspb_control;
};

/*******************************************************************************
 *
 * This class is used to create MacroSteps given a string that represents the
 * type of step and the arguments needed to create a MacroStep (a name and
 * a vector of arguments).
 *
 * Every MacroStep needs to have a MacroStepProxy that knows how to create
 * the MacroStep.  The proxy needs to register with this factory when it
 * is created.  A single static instance of the proxy should be created in the
 * same file that the proxy is defined.
 *
 ******************************************************************************/
class MacroStepFactory
{
	public:
		static void registerProxy(std::string ctrl_name, std::string type, MacroStepProxyBase * proxy);

		static MacroStep * create(std::string ctrl_name, std::string type, tinyxml2::XMLElement *xml);
};

/*******************************************************************************
 *
 * This template class defines a Proxy class for the specified type and
 * registers the MacroStep with the factor using the provided name
 * 
 *
 * Example:
 * 
 ******************************************************************************/
template <class MST>
class MacroStepProxy : public MacroStepProxyBase
{
	public:
		MacroStepProxy(std::string ctrl_name, std::string ms_name, void * control): MacroStepProxyBase(control)
		{
			//This is a compile time check make sure the template type is a MacroStep
			//((MST*)0);

			MacroStepFactory::registerProxy(ctrl_name, ms_name, (MacroStepProxyBase *)this);
		}

		~MacroStepProxy(void)
		{
		}

		MacroStep * create(std::string name, tinyxml2::XMLElement *xml)
		{
			return (MacroStep *)(new MST(name, xml, mspb_control));
		}
};





