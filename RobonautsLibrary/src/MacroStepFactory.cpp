/*******************************************************************************
 *
 * File: MacroStepFactor.h
 *
 * This file contains the definition of the MacroStepFactory class and an
 * explanation of how the MacroStep, MacroStepProxy, Macro, MacroController,
 * and MacroStepFactory work together.
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

#include "RobonautsLibrary/MacroStepFactory.h"

/*******************************************************************************
 * 
 * This map is a big part of the magic of this factory working with
 * self-referencing objects (that is this is what makes it so the factory 
 * can work without knowing what it can create until run-time).
 *  
 *  NOTE:
 *    This is a pointer to a map that doesn't get created until the first
 *    class registers to avoid an allocation race condition that can occure
 *    on some compilers.
 *    
 ******************************************************************************/
std::map<std::string, MacroStepProxyBase *> * factory_map = NULL;

/*******************************************************************************
 *
 * Register a Macro type proxy
 *
 ******************************************************************************/
void MacroStepFactory::registerProxy(std::string ctrl_name, std::string type, MacroStepProxyBase * proxy)
{
	if (factory_map == NULL)
	{
		factory_map = new std::map<std::string, MacroStepProxyBase *> ;
		factory_map->clear();
	}

	Advisory::pinfo("MacroStepFactory::registerProxy adding %s %s", ctrl_name.c_str(), type.c_str());

	factory_map->insert(std::pair<std::string, MacroStepProxyBase *> (ctrl_name + "_" + type, proxy));
}

/*******************************************************************************
 *
 * Use a proxy to create a Macro instance
 *
 ******************************************************************************/
MacroStep * MacroStepFactory::create(std::string ctrl_name, std::string type, tinyxml2::XMLElement *xml)
{
	std::map<std::string, MacroStepProxyBase *>::iterator ittr = factory_map->find(ctrl_name + "_" + type);

	if (ittr == factory_map->end())
	{
		return NULL;
	}
	MacroStepProxyBase *PPP = ittr->second;

	return PPP->create(ctrl_name + "_" + type, xml);
}

