/*******************************************************************************
 *
 * File: ControlFactor.h
 *
 * @see ControlFactory.h for more information about Macros
 *
 * Written by:
 * 	The Robonautss
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsControls/ControlFactory.h"

using namespace std;

/*******************************************************************************
 * 
 * This map holds an instance of the ProxyBase class for each type of
 * PeriodicControl that can be created by this factory.
 *    
 ******************************************************************************/
map<string, ControlProxyBase *> g_control_factory_map;

/*******************************************************************************
 *
 * Register a Macro type proxy
 *
 ******************************************************************************/
void ControlFactory::registerProxy(string type, ControlProxyBase * proxy)
{
    g_control_factory_map.insert(pair<string, ControlProxyBase *> (type, proxy));
}

/*******************************************************************************
 *
 * Use a proxy to create a Macro instance
 *
 ******************************************************************************/
ControlThread * ControlFactory::create(string type,
	tinyxml2::XMLElement *xml)

{
	map<string, ControlProxyBase *>::iterator ittr = g_control_factory_map.find(type);

	if (ittr == g_control_factory_map.end())
	{
		return nullptr;
	}

	ControlProxyBase *proxy = ittr->second;

	return proxy->create(xml);;
}
