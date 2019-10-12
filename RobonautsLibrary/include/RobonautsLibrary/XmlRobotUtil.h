/*******************************************************************************
 *
 * File: XmlRobotUtil.h
 * 
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#ifdef _2018

#include "SpeedController.h"
#include "Solenoid.h"
#include "Encoder.h"
#include "Compressor.h"
#include "Relay.h"
#include "Servo.h"
#include "DigitalOutput.h"

#include "Jaguar.h"
#include "Victor.h"
#include "VictorSP.h"
#include "Talon.h"

#else

#include "frc/SpeedController.h"
#include "frc/Solenoid.h"
#include "frc/Encoder.h"
#include "frc/Compressor.h"
#include "frc/Relay.h"
#include "frc/Servo.h"
#include "frc/DigitalOutput.h"

#include "frc/Jaguar.h"

#include "frc/Victor.h"
#include "frc/VictorSP.h"
#include "frc/Talon.h"

#endif

//#include "ctre/Phoenix.h"

#include "gsi/Thread.h"
#include "gsu/tinyxml2.h"

#include "RobonautsLibrary/RAnalogIrSensor.h"
#include "RobonautsLibrary/RAbsPosSensor.h"
#include "RobonautsLibrary/RDigitalInput.h"
#include "RobonautsLibrary/RPot.h"
#include "RobonautsLibrary/ScaledAnalog.h"
#include "RobonautsLibrary/RCounter.h"
#include "RobonautsLibrary/SimplePID.h"
#include "RobonautsLibrary/SimpleTrapCntl.h"
#include "RobonautsLibrary/RGyro.h"
#include "RobonautsLibrary/RDblSolenoid.h"
#include "RobonautsLibrary/RSpiGyro.h"
#include "RobonautsLibrary/RSpeedController.h"


/*******************************************************************************
 *
 * This class contains methods for parsing XML elements to create objects
 * for use by the robot.  This is intended to help keep the XML interface
 * consistent across many classes and to reduce the amount of duplicate 
 * code.
 * 
 ******************************************************************************/
class XmlRobotUtil
{
	public:
		static SpeedController *createSpeedController(tinyxml2::XMLElement* xml);
		static RSpeedController *createRSpeedController(tinyxml2::XMLElement* xml);
		static Solenoid *createSolenoid(tinyxml2::XMLElement* xml);
		static Encoder *createEncoder(tinyxml2::XMLElement *xml);
		static Compressor *createCompressor(tinyxml2::XMLElement *xml);
		
		static RAbsPosSensor *createAbsPosSensor(tinyxml2::XMLElement* xml);
		static RPot *createPot(tinyxml2::XMLElement* xml);
		static RGyro *createGyro(tinyxml2::XMLElement* xml);
		static RCounter *createCounter(tinyxml2::XMLElement* xml);
		static SimplePID *createPID(tinyxml2::XMLElement* xml);
		static SimpleTrapCntl *createTrapCntl(tinyxml2::XMLElement* xml);
		static TrapezoidalProfile *createTrapezoid(tinyxml2::XMLElement* xml);
		static WashoutCommand *createWashout(tinyxml2::XMLElement* xml);
		static Relay *createRelay(tinyxml2::XMLElement* xml);
		static Servo *createServo(tinyxml2::XMLElement* xml);
		static RDigitalInput *createDigitalInput(tinyxml2::XMLElement* xml);
		static DigitalOutput *createDigitalOutput(tinyxml2::XMLElement* xml);
		static ScaledAnalog *createScaledAnalog(tinyxml2::XMLElement* xml);
		static RDblSolenoid *createDblSolenoid(tinyxml2::XMLElement* xml);
		static RSpiGyro *createSpiGyro(tinyxml2::XMLElement* xml);
		static Gyro *createGyroAny(tinyxml2::XMLElement* xml);
		static RAnalogIrSensor *createAnalogIrSensor(tinyxml2::XMLElement* xml);

		static double getPeriod(tinyxml2::XMLElement *xml);
		static gsi::Thread::ThreadPriority getPriority(tinyxml2::XMLElement *xml);
};
