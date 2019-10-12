/*******************************************************************************
 *
 * File: CameraServerControl.cpp
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RobotUtil.h"
#include "RobonautsLibrary/XmlRobotUtil.h"
#include "gsu/Advisory.h"

#ifdef _2018
#include "smartdashboard/SmartDashboard.h" //WPI
#else
#include "frc/smartdashboard/SmartDashboard.h" //WPI
#endif

#include "RobonautsControls/CameraServerControl.h"

using namespace tinyxml2;
using namespace frc;

/*******************************************************************************	
 * 
 * Create an instance of a motor control and connect it to the specified
 * motor and inputs
 * 
 ******************************************************************************/
CameraServerControl::CameraServerControl(std::string control_name, XMLElement* xml)
	: ControlThread(control_name)
{
	XMLElement *comp;
	Advisory::pinfo("========================= Creating Camera Server Control [%s] =========================",
	            control_name.c_str());

	m_selected_camera = 0;

	const char *name = nullptr;

	//
	// Parse XML
	//
	comp = xml-> FirstChildElement("camera");
	while (comp != nullptr)
	{
        Advisory::pinfo("  initializing camera");

	    int camera_id = -1;

	    comp->QueryIntAttribute(("id"), &camera_id);
	    if ((camera_id >= 0) && (camera_id < 2))
	    {
            int camera_res_x = 160;
            int camera_res_y = 120;
            int camera_fps = 10;

            comp->QueryIntAttribute("res_x", &camera_res_x);
            comp->QueryIntAttribute("res_y", &camera_res_y);
            comp->QueryIntAttribute("fps", &camera_fps);

            m_camera[camera_id] = CameraServer::GetInstance()->StartAutomaticCapture(camera_id);
            m_camera[camera_id].SetResolution(camera_res_x, camera_res_y);
            m_camera[camera_id].SetFPS(camera_fps);

            // dummy sink to keep camera connection open
            m_video_sink[camera_id] = new cs::CvSink(m_camera[camera_id].GetName().c_str());
            m_video_sink[camera_id]->SetSource(m_camera[camera_id]);
            m_video_sink[camera_id]->SetEnabled(true);

            Advisory::pinfo("  camera %d set to %dx%d with %d fps", camera_id, camera_res_x, camera_res_y, camera_fps);
	    }
	    else
	    {
	        Advisory::pwarning("CameraServerControl ignoring camera specification with invalid \"id\" attribute of %d", camera_id);
	    }

		comp = comp->NextSiblingElement("camera");
	}

	comp = xml-> FirstChildElement("oi");
	while (comp != nullptr)
	{
		name = comp->Attribute("name");
		if (name != nullptr)
		{
			if (strcmp(name, "toggle") == 0)
			{
				Advisory::pinfo("  connecting toggle channel");
				OIController::subscribeDigital(comp, this, CMD_TOGGLE);
			}
			else if (strcmp(name, "select") == 0)
			{
				Advisory::pinfo("  connecting select channel");
				OIController::subscribeDigital(comp, this, CMD_SELECT);
			}
			else
			{
                Advisory::pinfo("  unknown oi ignored name=\"%s\"", name);
			}
		}
		
		comp = comp->NextSiblingElement("oi");
	}

    m_video_server = CameraServer::GetInstance()->GetServer();
}

/*******************************************************************************	
 * 
 * Release any resources allocated by this object
 * 
 ******************************************************************************/
CameraServerControl::~CameraServerControl(void)
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void CameraServerControl::controlInit(void)
{
}

/*******************************************************************************
 *
 * This is the callback for OIController::subscribeAnalog, if the XML config 
 * specifies an analog input, the constructor of this object will connect that 
 * input to this method.
 * 
 * @param id	the control id passed to the subscribe
 * @param val	the new value of the analog channel that was subscribed to
 * 
 ******************************************************************************/
void CameraServerControl::setAnalog(int id, float val)
{
}

/*******************************************************************************	
 *
 * This is the callback for OIController::setDigital, if the XML config 
 * specifies an increment, decrement, or stop input, the constructor 
 * of this object will connect that/those input(s) to this method.
 * 
 * @param id	the control id passed to the subscribe
 * @param val	the new value of the digital channel that was subscribed to
 * 
 ******************************************************************************/
void CameraServerControl::setDigital(int id, bool val)
{
//    Advisory::pinfo("CameraServerControl::setDigital(%d, %s)", id, (val?"true":"false"));
	int previous_selected = m_selected_camera;

	switch (id)
	{
		case CMD_TOGGLE:
		{
			if (val)
			{
			    if (m_selected_camera == 0)
			    {
			        m_selected_camera = 1;
			    }
			    else
			    {
			        m_selected_camera = 0;
			    }
			    if (m_selected_camera != previous_selected)
			    {
			    	Advisory::pinfo("selecting camera %d, %s, %s, %s, %s", m_selected_camera,
			        m_camera[m_selected_camera].GetName().c_str(), m_camera[m_selected_camera].GetPath().c_str(),
			        m_camera[m_selected_camera].IsConnected()?"true":"false", m_camera[m_selected_camera].GetDescription().c_str());
			    }

			    m_video_server.SetSource(m_camera[m_selected_camera]);
//	            NetworkTable::GetTable("")->PutString("CameraSelection", m_camera[m_selected_camera].GetName());
//			    CS_Status set_status = m_video_server.GetLastStatus();
//			    cs::VideoSource check_source = m_video_server.GetSource();
//			    Advisory::pinfo("checking camera x, %s, %s, %s  status=%d",
//			        check_source.GetName().c_str(),
//			        check_source.IsConnected()?"true":"false", check_source.GetDescription().c_str(), set_status);
			}
		} break;
			
		case CMD_SELECT:
		{
			if (val)
			{
	            m_selected_camera = 1;
			}
			else
			{
			    m_selected_camera = 0;
			}
			if (m_selected_camera != previous_selected)
			{
				Advisory::pinfo("selecting camera %d, %s, %s, %s, %s", m_selected_camera,
		        m_camera[m_selected_camera].GetName().c_str(), m_camera[m_selected_camera].GetPath().c_str(),
		        m_camera[m_selected_camera].IsConnected()?"true":"false", m_camera[m_selected_camera].GetDescription().c_str());
			}

		    m_video_server.SetSource(m_camera[m_selected_camera]);
//		    NetworkTable::GetTable("")->PutString("CameraSelection", m_camera[m_selected_camera].GetName());
//            CS_Status set_status = m_video_server.GetLastStatus();
//            cs::VideoSource check_source = m_video_server.GetSource();
//            Advisory::pinfo("checking camera x, %s, %s, %s  status=%d",
//                check_source.GetName().c_str(),
//                check_source.IsConnected()?"true":"false", check_source.GetDescription().c_str(), set_status);
		} break;

		default:
			break;
	}

}

/*******************************************************************************
 *
 ******************************************************************************/
void CameraServerControl::setInt(int id, int val)
{
}

/*******************************************************************************	
 *
 ******************************************************************************/
void CameraServerControl::publish()
{
    SmartDashboard::PutNumber(getName() +" selected camera: ", m_selected_camera);
}

/*******************************************************************************
 *
 * This method will be called once a period to do anything that is needed,
 * in this case it just sets the motor power to the current value.
 * 
 ******************************************************************************/
void CameraServerControl::run()
{
    while(!isStopRequested())
    {
        sleep(1.0);
    }
}
