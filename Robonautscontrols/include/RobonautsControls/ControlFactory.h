/*******************************************************************************
 *
 * File: ControlFactor.h
 * 
 * This file contains the definition of the ControlProxyBase, the
 * PeriodicControlProxy template, and the ControlFactory classes.
 *
 * 
 *  Written by:
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

#include "RobonautsLibrary/XmlRobotUtil.h"
#include "RobonautsControls/ControlThread.h"
#include "RobonautsControls/PeriodicControl.h"

/*******************************************************************************
 *
 * The ContrlProxy class is the base class for all proxy classes.  These
 * proxy classes are used to register a Control classes with the factory so
 * instances can be created via the factory.
 * 
 * Most Control classes will be able to use the c preprocessor macro
 * defined below to declare and create a single proxy instance.  However, if
 * the Control requires special construction a separate proxy that
 * extends this base class can be declared and instantiated.
 * 
 ******************************************************************************/
class ControlProxyBase
{
	public:
		ControlProxyBase(void)	{}
		virtual ~ControlProxyBase(void) {}
		virtual ControlThread * create(tinyxml2::XMLElement *xml) = 0;
};

/*******************************************************************************
 *
 *
 *
 ******************************************************************************/
template <class PCB>
class ControlProxy : public ControlProxyBase
{
	public:
		ControlProxy(): ControlProxyBase()	{}
		~ControlProxy(void) {}

		ControlThread * create(tinyxml2::XMLElement *xml)
		{
			ControlThread *pc = nullptr;

			const char *control_name = xml->Attribute("name");
			if (control_name == nullptr)
			{
				control_name = "unnamed";
			}

            pc = (ControlThread *)(new PCB(std::string(control_name), xml));

            PeriodicControl *pcp = static_cast<PeriodicControl *>(pc);
            if (pc != nullptr)
            {
                double period = 0.0;
                xml->QueryDoubleAttribute("period", &period);
                if (period != 0.0)
                {
                    pcp->setPeriod(period);
                }
            }
			return pc;
		}
};

/*******************************************************************************
 *
 * This class is used to create PacbotControls given a string that represents
 * the type of control and a pointer to an XML structure that contains the
 * definition needed to configure the control.
 *
 * Every PacbotControll needs to have a PacbotControlProxy that knows how to 
 * create the PacbotControl.  The proxy needs to register with this factory 
 * when it is created.  A single static instance of the proxy should be 
 * created in the same file that the proxy is defined.
 *
 ******************************************************************************/
class ControlFactory
{
	public:
		static void registerProxy(std::string type, ControlProxyBase * proxy);
		static ControlThread * create(std::string type, tinyxml2::XMLElement *xml);
};
