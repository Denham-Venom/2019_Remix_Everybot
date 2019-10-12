/*******************************************************************************
 *
 * File: CameraServerControl.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include "RobonautsControls/PeriodicControl.h"
#include "RobonautsLibrary/OIObserver.h"
#include "RobonautsLibrary/OIController.h"

#include "cscore.h"
#include "cameraserver/CameraServer.h"

/*******************************************************************************	
 * 
 * Create an instance of camera server control and connect it to the specified
 * user inputs
 * 
 * This class is designed to be created from an XML element with the following
 * format, portions contained in [ ] are optional.
 *  
 *  <control type="camera_server" >
 *      [<camera id="0" [res_x="160"] [res_y="120"] [fps="15"] />]
 * 		[<camera id="1" [res_x="160"] [res_y="120"] [fps="15"] />]
 *
 *      [<oi name="toggle"	device="pilot" chan="1" [invert="false"] />]
 *      [<oi name="select"	device="pilot" chan="5" [invert="false"] />]
 *  </control>
 *
 ******************************************************************************/
class CameraServerControl : public ControlThread, public OIObserver
{
	public:
		enum {CMD_TOGGLE=0, CMD_SELECT};
		
		CameraServerControl(std::string control_name, tinyxml2::XMLElement *xml);
		~CameraServerControl(void);

  		void controlInit(void);

		void setAnalog(int id, float val);
		void setDigital(int id, bool val);
		void setInt(int id, int val);

		void publish(void);

		void run(void);

	private:
		cs::UsbCamera m_camera[2];
		cs::CvSink * m_video_sink[2];
		cs::VideoSink m_video_server;
		uint8_t m_selected_camera;
};
