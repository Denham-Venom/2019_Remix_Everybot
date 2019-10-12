#include <math.h>

#include "gsu/Advisory.h"

//#include "frc/WPILib.h"

#include "RobonautsLibrary/XmlRobotUtil.h"

using namespace frc;

/*******************************************************************************
 *
 * Create an instance of a speed controller from the given XML element.
 * 
 * The XML element must have the format of
 *  
 *  <motor [type="type"] [port="port"] [invert="false"] [...] >
 *  
 *  Where:	type 	the type of speed controller being used to drive this
 *  				motor, can be Jaguar, Talon, or Victor, VictorSP, TalonSRX,
 *  				or CanTalon if a type is not
 *  				specified, the default value of Victor will be used.
 *  		
 *  		port	the port to which this speed controller is connected,
 *  				can be a value from 0 to 63, if not specified, the
 *  				default value of 1 is used.
 *
 *  	    invert  the direction of the motor will be reversed from the
 *  	            command if invert is true, default is false
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="left"
 *  				
 ******************************************************************************/
SpeedController *XmlRobotUtil::createSpeedController(tinyxml2::XMLElement* xml)
{
	Advisory::pwarning("Using deprecated createSpeedController method");

    SpeedController *sc = nullptr;

	const char *type 	= xml->Attribute("type");
	int port	= 1;
	bool invert = false;

	xml->QueryIntAttribute("port", &port);
	xml->QueryBoolAttribute("invert", &invert);
	
	if ((port < 0) || (port > 63))
	{
		port = 1;
	}
	
    Advisory::pinfo("    creating %s Speed Controller on port %d, invert=%s", type, port, invert?"true":"false");

    if      (0 == strcmp(type, "Jaguar"))       sc = new Jaguar(port);
	else if (0 == strcmp(type, "Talon"))        sc = new Talon(port);
	else if (0 == strcmp(type, "Victor"))	    sc = new Victor(port);
	else if (0 == strcmp(type, "VictorSP"))	    sc = new VictorSP(port);
//	else if (0 == strcmp(type, "TalonSRX"))	    sc = new TalonSRX(port);
//	else if (0 == strcmp(type, "CanTalon"))	    sc = new CANTalon(port);
//	else                            		    sc = new TalonSRX(port);

	if (nullptr != sc)
	{
	    sc->SetInverted(invert);
	}

    return sc;
}

/*******************************************************************************
 *
 * Create an instance of a robonauts speed controller from the given XML element.
 * Initially, it supports all controllers, with extensive CANTalon support, and
 * minimal support for everything else.  Except for motor and sensor inversions, the
 * XML does not features such as gains and initial modes,
 *
 * For a CAN speed controller the XML element must have the format of
 *
 *  <motor type="CanTalon" [device="port"] [control_period="10"] [invert="false"]  >
 *      [<encoder  [invert="false"] [scale="1.0"] />]
 *      [<pid [kf="0.001"] [kp="0.0"] [kd="0.0"] [ki="0.0"] />]
 *      []
 *  </motor>
 *
 * For a PWM speed controller the XML must have the format of
 *   <motor type="Victor" [device="port"] [fuse="1"] [control_period="10"] [invert="false"] />
 *
 *  Where:	type 	the type of speed controller being used to drive this
 *  				motor, can be Jaguar, Talon, or Victor, VictorSP, TalonSRX,
 *  				or CanTalon if a type is not
 *  				specified, the default value of Victor will be used.
 *
 *          FOR CAN TALON
 *  		device	the device id of the CAN Talon
 *  				can be a value from 0 to 63, if not specified, the
 *  				default value of 1 is used.
 *
 *  	    invert_motor
 *  	    	the direction of the motor will be reversed from the
 *  	        command if invert is true, default is false
 *
 *  		invert_sensor
 *  			invert the sensor feedback, default is false
 *  			at time of development, invert sensor does not appear to
 *  			work on CANTalon, managed in user class

 *  		scale
 *  			number of output units per input unit.  For example,
 *  		 	number of inches per count on a lift
 *
 *			control_period
 *				Control period in millisecs 1 is 1000hz, 10 is 100 hz, etc
 *
 *			normally_open
 *              the limit switch is normally open if true, normally closed
 *              if false
 * 
 *          reset_zero
 *              if this limit switch is pressed, the position will reset to
 *              zero
 * 
 *  		In the future, will(may) be expanded for keeping external sensors
 *  		within the class and perhaps PID
 *
 *			Note: by default, CAN speed controllers come up in duty cycle mode
 *			If the application only wants to use duty cycle mode, the
 *			createSpeedController function should be used as it is simpler
 *			and refers to the common Set, Get, Invert, Disable, etc available
 *			through the SpeedController common interface.  To use the more
 *			advanced CANTalon features such as motion profiling, a pure CANTalon
 *			class should be created.
 *
 ******************************************************************************/
RSpeedController *XmlRobotUtil::createRSpeedController(tinyxml2::XMLElement* xml)
{
	RSpeedController *sc = nullptr;

    int device  = 1;
    xml->QueryIntAttribute("port", &device);
	if ((device < 0) || (device > 63))
	{
		device = 1;
	}

    int ctrl_period=10;
    xml->QueryIntAttribute("control_period", &ctrl_period);

    const char *type    = xml->Attribute("type");
    if ((strcmp("CanTalon", type) == 0)  || (strcmp("TalonSrxCan", type) == 0))
	{
	    sc = new RSpeedControllerCan(device, ctrl_period);

	    bool invert_motor = false;
	    bool invert_sensor = false;
		bool brake_mode = false;

	    int current_limit = 0;
	    int peak_current = 0;
	    int peak_current_dur = 0;

        float peak_forward_voltage = 12.0;
        float peak_reverse_voltage = -12.0;
        float nominal_forward_voltage = 0.0;
        float nominal_reverse_voltage = 0.0;

	    double scale=1.0;
	    if(nullptr != sc)
	    {
	        xml->QueryBoolAttribute("invert", &invert_motor);
	        sc->InvertMotor(invert_motor);

	        xml->QueryBoolAttribute("brake_mode", &brake_mode);
			sc->SetBrakeMode(brake_mode);

	        xml->QueryIntAttribute("current_limit", &current_limit);
	        xml->QueryIntAttribute("peak_current", &peak_current);
	        xml->QueryIntAttribute("peak_current_dur", &peak_current_dur);

	        sc->SetCurrentLimit(current_limit, peak_current, peak_current_dur);

	        tinyxml2::XMLElement *elem;
	        if (current_limit != 0 || peak_current != 0 || peak_current_dur != 0)
	        {
	        	sc->SetCurrentLimitEnabled(true);
	        }

	        xml->QueryFloatAttribute("peak_forward_voltage", &peak_forward_voltage);
	        xml->QueryFloatAttribute("peak_reverse_voltage", &peak_reverse_voltage);
	        xml->QueryFloatAttribute("nominal_forward_voltage", &nominal_forward_voltage);
	        xml->QueryFloatAttribute("nominal_reverse_voltage", &nominal_reverse_voltage);

	        sc->SetClosedLoopOutputLimits(nominal_forward_voltage, nominal_reverse_voltage,
	            peak_forward_voltage, peak_reverse_voltage);

	        elem = xml->FirstChildElement("encoder");
	        if (elem != nullptr)
	        {
	            sc->SetFeedbackDevice(RSpeedController :: INTERNAL_ENCODER);

	            elem->QueryDoubleAttribute("scale", &scale);
	            sc->SensorOutputPerCount(scale);
	            bool is_drive=false;

	            elem->QueryBoolAttribute("drive", &is_drive);

	            elem->QueryBoolAttribute("invert", &invert_sensor);
                sc->InvertSensor(invert_sensor, is_drive);
	        }
			else
			{
				elem = xml->FirstChildElement("pot");
				if (elem != nullptr)
				{
					bool sensor_wrap = false;
					elem->QueryBoolAttribute("sensor_wrap", &sensor_wrap);

					sc->SetFeedbackDevice(RSpeedController :: INTERNAL_POTENTIOMETER, (sensor_wrap?1:0));

					elem->QueryBoolAttribute("invert", &invert_sensor);
					sc->InvertSensor(invert_sensor);

					float raw1 	= 0.0;
					float raw2  = 5.0;
					float calc1 = 0.0;
					float calc2 = 5.0;

					float scale = 1.0;
					float offset = 0.0;

					elem->QueryFloatAttribute("p1_raw", &raw1);
					elem->QueryFloatAttribute("p2_raw", &raw2);
					elem->QueryFloatAttribute("p1_cal", &calc1);
					elem->QueryFloatAttribute("p2_cal", &calc2);

					scale	= (calc2 - calc1)/(raw2 - raw1);
					offset 	= calc1-raw1*scale;
					sc->SensorOutputPerCount(scale, offset);

					Advisory::pinfo("  motor controller set to internal pot, sensor_wrap=%s, invert=%s, p1(%f,%f) p2(%f,%f), scale=%f offset=%f",
							sensor_wrap?"true":"false", invert_sensor?"true":"false", raw1, calc1, raw2, calc2, scale, offset);
				}
			}

	        bool has_upper_limit = false;
	        bool has_lower_limit = false;
			bool upper_normally_open=false;
			bool lower_normally_open=false;
			bool upper_reset_zero=false;
			bool lower_reset_zero=false;

	        elem = xml->FirstChildElement("digital_input");
			while (elem != nullptr)
			{
			    const char *sw_name    = elem->Attribute("name");
			    if (strcmp("upper_limit", sw_name) == 0)
				{
			    	elem->QueryBoolAttribute("normally_open", &upper_normally_open);
			    	elem->QueryBoolAttribute("reset_zero", &upper_reset_zero);
					sc->SetForwardLimitSwitch(upper_normally_open, true, upper_reset_zero);

			    	has_upper_limit = true;
				}
			    else if (strcmp("lower_limit", sw_name) == 0)
			    {
			    	elem->QueryBoolAttribute("normally_open", &lower_normally_open);
			    	elem->QueryBoolAttribute("reset_zero", &lower_reset_zero);
					sc->SetReverseLimitSwitch(lower_normally_open, true, lower_reset_zero);

			    	has_lower_limit = true;
			    }
			    else
			    {
			    	Advisory::pinfo ("motor limit switch unknown name of %s found", sw_name);
			    }

				elem = elem->NextSiblingElement("digital_input");
			}

			// if the limits were set in a previous configuration,
			// we should disable them now
            if (! has_upper_limit)
            {
                sc->SetForwardLimitSwitch(false, false, false);
            }

            if (! has_lower_limit)
            {
                sc->SetReverseLimitSwitch(false, false, false);
            }

            double kf = 0.001;
            double kp = 0.0;
            double ki = 0.0;
            double kd = 0.0;
            double iz = 0.0;
            double cruise_velocity = 0.0;
            double acceleration = 0.0;

            elem = xml->FirstChildElement("pid");
	        if (elem != nullptr)
	        {
				elem->QueryDoubleAttribute("kf", &kf);
				elem->QueryDoubleAttribute("kp", &kp);
				elem->QueryDoubleAttribute("ki", &ki);
				elem->QueryDoubleAttribute("kd", &kd);
				elem->QueryDoubleAttribute("iz", &iz);
				elem->QueryDoubleAttribute("cruise_velocity", &cruise_velocity);
				elem->QueryDoubleAttribute("acceleration", &acceleration);

	            sc->SetF(kf);
	            sc->SetP(kp);
	            sc->SetI(ki);
	            sc->SetD(kd);

	            if (iz != 0.0)
	            {
	            	sc->SetIZone(iz);
	            }

	            if (cruise_velocity != 0.0)
	            {
	            	sc->SetCruiseVelocity(cruise_velocity);
	            }

	            if (acceleration != 0.0)
	            {
	            	sc->SetAcceleration(acceleration);
	            }
	        }

	        Advisory::pinfo("    creating CAN Talon Speed Controller on device %d, invert_m=%s, invert_s=%s scale=%.3f, kf=%f, kp=%f, ki=%f, kd=%f, cruise_velocity=%f, acceleration=%f",
	        	device, invert_motor?"true":"false",invert_sensor?"true":"false", scale, kf, kp, ki, kd, cruise_velocity, acceleration);
	    }
	    else
	    {
	        Advisory::pinfo("    failed to create speed controller of type \"%s\"", type);
	    }
	}
	else if (strcmp("VictorSpxCan", type) == 0)
	{

	    sc = new RSpeedControllerVictorSpxCan(device, ctrl_period);

	    if(nullptr != sc)
	    {
	        Advisory::pinfo("    creating %s Speed Controller", type);

    	    bool invert_motor = false;
			bool brake_mode = false;

	        xml->QueryBoolAttribute("invert", &invert_motor);
	        sc->InvertMotor(invert_motor);

	        xml->QueryBoolAttribute("brake_mode", &brake_mode);
			sc->SetBrakeMode(brake_mode);
	    }
	    else
	    {
	        Advisory::pinfo("    failed to create speed controller of type \"%s\"", type);
	    }
	}
    else
    {
    	RSpeedControllerPwm * sc_pwm = nullptr;
        if (strcmp("Talon", type) == 0)
        {
        	sc_pwm = new RSpeedControllerPwm(new Talon(device), ctrl_period);
        }
        else if (strcmp("Victor", type) == 0)
        {
        	sc_pwm = new RSpeedControllerPwm(new Victor(device), ctrl_period);
        }
        else if (strcmp("VictorSP", type) == 0)
        {
        	sc_pwm = new RSpeedControllerPwm(new VictorSP(device), ctrl_period);
        }
        else if (strcmp("Jaguar", type) == 0)
        {
        	sc_pwm = new RSpeedControllerPwm(new Jaguar(device), ctrl_period);
        }
//        else if (strcmp("TalonSrxCan", type) == 0)
//        {
//        	sc_pwm = new RSpeedControllerPwm(new TalonSRX(device), ctrl_period);
//        }



        sc = sc_pwm;

        if(nullptr != sc)
        {
            bool invert_motor = false;
            int fuse = -1;

            xml->QueryBoolAttribute("invert", &invert_motor);
            sc->InvertMotor(invert_motor);

            xml->QueryIntAttribute("fuse", &fuse);
            sc_pwm->SetFuse(fuse);

            Advisory::pinfo("    creating Speed Controller on port %d, invert_m=%s", device, invert_motor?"true":"false");
        }
        else
        {
            Advisory::pinfo("    failed to create speed controller of type \"%s\"", type);
        }
    }


	return sc;
}


/*******************************************************************************
 *
 * Create an instance of a solenoid from the given XML element.
 * 
 * The XML element must have the format of
 *  
 *  <solenoid [module="module"] [port="port"] [...] >
 *  
 *  Where:	module	the module (or card) to which this speed controller is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 0 will be used (FRC_CRIO Only)
 *  				
 *  		port	the port to which this speed controller is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
Solenoid *XmlRobotUtil::createSolenoid(tinyxml2::XMLElement* xml)
{
	int module = xml->IntAttribute("module");
	if ((module < 0) || (module > 63))
	{
		Advisory::pinfo("    using default \"module\" of 0");
		module = 0;
	}
	
	int port = xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		Advisory::pinfo("    using default \"port\" of 1");
		port = 1;
	}

	if (module > 0)
	{
		Advisory::pinfo("    creating solenoid on module %d, port %d", module, port);
		return new Solenoid(module, port);
	}
	else
	{
		Advisory::pinfo("    creating solenoid on port %d", port);
		return new Solenoid(port);
	}
}

/*******************************************************************************
 *
 * Create an instance of a double solenoid from the given XML element.
 *
 * The XML element must have the format of
 *
 *  <dbl_solenoid [module="module"] [port_a="port"] [port_b="port"] [...] >
 *
 *  Where:	module	the module (or card) to which this speed controller is
 *  				connected, can be a vaule of 1 or 2, if a module is not
 *  				specified, the default value of 0 will be used (FRC_CRIO Only)
 *
 *  		port_a	the port to which the first solenoid is conncected
 *
 *  		port_b	the port to which the second solenoid is connected
 *
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *
 ******************************************************************************/
RDblSolenoid *XmlRobotUtil::createDblSolenoid(tinyxml2::XMLElement* xml)
{
	int module = 0;
	int port_a = 0;
	int port_b = 1;

	xml->QueryIntAttribute("module", &module);
	xml->QueryIntAttribute("port_a", &port_a);
	xml->QueryIntAttribute("port_b", &port_b);
	if ((module < 0) || (module > 63))
	{
		Advisory::pinfo("    using default \"module\" of 0");
		module = 0;
	}

	if ((port_a < 0) || (port_a > 7))
	{
		Advisory::pinfo("    using default \"port_a\" of 0");
		port_a = 0;
	}

	if ((port_b < 0) || (port_b > 7))
	{
		Advisory::pinfo("    using default \"port_b\" of 1");
		port_b = 1;
	}


	Advisory::pinfo("    creating double solenoid on module %d, ports %d, %d", module, port_a, port_b);
	return new RDblSolenoid(module, port_a, port_b);
}

/*******************************************************************************
 *
 * Create an instance of a double solenoid from the given XML element.
 *
 * The XML element must have the format of
 *
 ******************************************************************************/
RSpiGyro *XmlRobotUtil::createSpiGyro(tinyxml2::XMLElement* xml)
{
    SPI::Port port=SPI::kMXP;
    float sample_period = 0.001;
    int spi_cmd = 0x20000000u;
    int xfer_size = 4;
    int valid_mask = 0x0c00000eu; // ADXRS450 is 0x0c000000u
    int valid_value = 0x04000000u;
    int data_shift = 10u;
    int data_size = 16u;
    bool is_signed = true;
    bool big_endian = true;
    bool invert = false;

    const char *port_name = xml->Attribute("port");
         if (strcmp(port_name, "MXP") == 0) port = SPI::kMXP;
    else if (strcmp(port_name, "CS0") == 0) port = SPI::kOnboardCS0;
    else if (strcmp(port_name, "CS1") == 0) port = SPI::kOnboardCS1;
    else if (strcmp(port_name, "CS2") == 0) port = SPI::kOnboardCS2;
    else if (strcmp(port_name, "CS3") == 0) port = SPI::kOnboardCS3;

    xml->QueryFloatAttribute("sample_period", &sample_period);
    xml->QueryIntAttribute("spi_cmd", &spi_cmd);
    xml->QueryIntAttribute("xfer_size", &xfer_size);
    xml->QueryIntAttribute("valid_mask", &valid_mask);
    xml->QueryIntAttribute("valid_value", &valid_value);
    xml->QueryIntAttribute("data_shift", &data_shift);
    xml->QueryIntAttribute("data_size", &data_size);

    xml->QueryBoolAttribute("is_signed", &is_signed);
    xml->QueryBoolAttribute("big_endian", &big_endian);
    xml->QueryBoolAttribute("invert", &invert);

    RSpiGyro *rg = new RSpiGyro(port, (double)sample_period, (uint32_t)spi_cmd,
        (uint8_t)xfer_size, (uint32_t)valid_mask, (uint32_t)valid_value,
        (uint8_t)data_shift, (uint8_t)data_size, is_signed, big_endian, invert);

    return(rg);
}

/*******************************************************************************
 *
 * Create a encoder
 * 
 * The XML element must have the format of
 *  
 *  <encoder [module="1"] [port_a="1"] [port_b="2"] [invert="false"] />
 *  
 *  Where:	module	the module (or card) to which this speed controller is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 1 will be used (FRC_CRIO Only)
 *  				
 *  		port_a	one of the digital input ports to which this encoder is 
 *                  conncected, can be a value from 1 to 14, if not specified, 
 *                  the default value of 1 is used.
 *  				
 *  		port_b	one of the digital input ports to which this encoder is 
 *                  conncected, can be a value from 1 to 14, if not specified, 
 *                  the default value of 2 is used.
 *  				
 *  		invert	if true, the inputs will be reversed in software to
 *                  change the direction the encoder counts, default is false
 *  				
 ******************************************************************************/
Encoder *XmlRobotUtil::createEncoder(tinyxml2::XMLElement* xml)
{
	Encoder *encoder;
	
#if defined (FRC_CRIO)
	int module	= xml->IntAttribute("module");
	if ((module < 1) || (module > 2))
	{
		Advisory::pinfo("    using default \"module\" of 1");
		module = 1;
	}
#endif
	
	int port_a	= xml->IntAttribute("port_a");
	if ((port_a < 0) || (port_a > 20))
	{
		Advisory::pinfo("    using default \"port_a\" of 1");
		port_a = 1;
	}
	
	int port_b  = xml->IntAttribute("port_b");
	if ((port_b < 0) || (port_b > 20))
	{
		Advisory::pinfo("    using default \"port_a\" of 2");
		port_b = 2;
	}
	
	bool invert = xml->BoolAttribute("invert");

	float scale = xml->FloatAttribute("scale");
	if (scale == 0.0)
	{
		Advisory::pinfo("    using default \"scale\" of 0.01");
		scale = 0.01;	
	}
	
#if defined (FRC_CRIO)
	Advisory::pinfo("    creating encoder on module %d, port_a %d, port_b %d, escale=%f, invert %s",
			module, port_a, port_b, scale, invert?"true":"false");
	
	encoder = new Encoder(module, port_a, module, port_b, invert);
#else
	Advisory::pinfo("    creating encoder on port_a %d, port_b %d, escale=%f, invert %s",
			port_a, port_b, scale, invert?"true":"false");

	encoder = new Encoder(port_a, port_b, invert);
#endif
	
	encoder->SetDistancePerPulse(scale);

	return encoder;
}

/*******************************************************************************
 *
 * @TODO: look into adding RoboRIO PWM ID value
 *
 * < ... >
 *      <switch module="1" port="1" /> (FRC_CRIO Only)
 *      <relay  module="1" port="1" /> (FRC_CRIO Only)
 *  < ... >
 * 
 ******************************************************************************/
Compressor *XmlRobotUtil::createCompressor(tinyxml2::XMLElement *xml)
{
	if (xml) {} // stops unused variable warning

#if defined (FRC_CRIO)
	tinyxml2::XMLElement *comp;

	int switch_module = -1;
	int switch_port = -1;;

	int relay_module = -1;
	int relay_port = -1;
	
	comp = xml->FirstChildElement("switch");
	if (comp != NULL)
	{
		switch_module = comp->IntAttribute("module");	
		switch_port = comp->IntAttribute("port");
	}
	
	if ((switch_module < 1) || (switch_module > 2))
	{
		Advisory::pinfo("  using default \"module\" of 1 for \"switch\"");
		switch_module = 1;
	}
	
	if ((switch_port < 1) || (switch_port > 14))
	{
		Advisory::pinfo("  using default \"port\" of 14 for \"switch\"");
		switch_port = 14;
	}
	
	comp = xml->FirstChildElement("relay");
	if (comp != NULL)
	{
		relay_module = comp->IntAttribute("module");
		relay_port = comp->IntAttribute("port");
	}
	
	if ((relay_module < 1) || (relay_module > 2))
	{
		Advisory::pinfo("  using default \"module\" of 1 for \"relay\"");
		relay_module = 1;
	}
	
	if ((relay_port < 1) || (relay_port > 8))
	{
		Advisory::pinfo("  using default \"port\" of 1 for \"relay\"");
		relay_port = 1;
	}
	
	Advisory::pinfo("  creating compressor");
	Advisory::pinfo("    switch on module %d, port %d",
		switch_module, switch_port);
	Advisory::pinfo("    relay on module %d, port %d",
		relay_module, relay_port);

	return (new Compressor(switch_module, switch_port, relay_module,
		relay_port));
#else
	Advisory::pinfo("  creating compressor");
	return (new Compressor());
#endif
}

/*******************************************************************************
 *
 * Create a RAbsPosSensot
 *
 * The XML element must have the format of
 *
 *  <aps [port="port"] [raw1="0.0"] [calc1="0.0"] [raw2="5.0"] [calc2="5.0"]
 *       [wraps="0"] [raw_range="5.0"] [...] / >
 *
 *  Where:	port	the port to which this speed controller is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *
 *			p1_raw	the raw value at a given point, defaults to 0.0
 *
 *			p1_cal	what the calculated value should be at the raw1 point,
 *					defaults to 0.0
 *
 *			p2_raw	the raw value at a second point, defaults to 5.0
 *
 *			p2_cal	what the calculated value should be at the raw2 point,
 *					defaults to 5.0
 *
 *			wraps	the number of wraps of the sensor between raw1 and raw2,
 *					defaults to 0
 *
 *			raw_range	the range of the raw value, defaults to 5.0
 *
 ******************************************************************************/
RAbsPosSensor *XmlRobotUtil::createAbsPosSensor(tinyxml2::XMLElement* xml)
{
	RAbsPosSensor *aps;

	int port	= xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		// don't default to 1, 1 must be used by gyros
		Advisory::pinfo("    using default \"port\" of 2");
		port = 2;
	}

	Advisory::pinfo("    creating APS on port %d", port);
	aps = new RAbsPosSensor(port);

	if (aps != NULL)
	{
		float raw1 	= 0.0;
		float raw2  = 5.0;
		float calc1 = 0.0;
		float calc2 = 5.0;
		float wraps = 0.0;

		float scale = 1.0;
		float offset = 0.0;
		float raw_range = aps->getRawRange();

		xml->QueryFloatAttribute("p1_raw", &raw1);
		xml->QueryFloatAttribute("p2_raw", &raw2);
		xml->QueryFloatAttribute("p1_cal", &calc1);
		xml->QueryFloatAttribute("p2_cal", &calc2);
		xml->QueryFloatAttribute("wraps", &wraps);
		xml->QueryFloatAttribute("raw_range", &raw_range);

		scale	= (calc2 - calc1)/((raw2 - raw1) + (wraps * raw_range));
		offset 	= calc1-raw1*scale;

		aps->setScale(scale);
		aps->setOffset(offset);
		aps->setRawRange(raw_range);
	}

	return aps;
}

/*******************************************************************************
 *
 * Create a RPot
 * 
 * The XML element must have the format of
 *  
 *  <pot [module="module"] [port="port"] [scale="scale"] [offset="offset"]
 *    [min_raw="min_raw"] [max_raw="max_raw"] [...] >
 *  
 *  Where:	module	the module (or card) to which this speed controller is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 1 will be used (FRC_CRIO Only)
 *  				
 *  		port	the port to which this speed controller is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *  				
 *  		scale	the scale that will be applied to the raw pot value
 *  				using val = (scale * raw) + offset
 *  				
 *  		offset  the offset that will be applied to the raw value
 *  				using val = (scale * raw) + offset
 *  				
 *  		min_raw	the minimum raw value that will be considered as 
 *  				valid, if the raw value is less than this, isReady()
 *  				will return false.  This is to help detect failed
 *  				sensors.
 *  				
 *  		max_raw	the maximum raw value that will be considered as 
 *  				valid, if the raw value is greater than this, isReady()
 *  				will return false.  This is to help detect failed
 *  				sensors.
 *  				
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
RPot *XmlRobotUtil::createPot(tinyxml2::XMLElement* xml)
{
	RPot *pot;
	
#if defined (FRC_CRIO)
	int module	= xml->IntAttribute("module");
	if ((module < 1) || (module > 2))
	{
		Advisory::pinfo("    using default \"module\" of 1");
		module = 1;
	}
#endif
	
	int port	= xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		// don't default to 1, 1 must be used by gyros
		Advisory::pinfo("    using default \"port\" of 2");
		port = 2;
	}
	
#if defined (FRC_CRIO)
	Advisory::pinfo("    creating pot on module %d, port %d", module, port);
	pot = new RPot(module, port);
#else
	Advisory::pinfo("    creating pot on port %d", port);
	pot = new RPot(port);
#endif
	
	if (pot != NULL)
	{
		float raw1 	= 0.0;
		float raw2  = 5.0;
		float calc1 = 0.0;
		float calc2 = 5.0;

		float scale = 1.0;
		float offset = 0.0;

		float min_raw = 0.05;
		float max_raw = 4.95;

		xml->QueryFloatAttribute("min_raw", &min_raw);
		xml->QueryFloatAttribute("max_raw", &max_raw);

		xml->QueryFloatAttribute("p1_raw", &raw1);
		xml->QueryFloatAttribute("p2_raw", &raw2);
		xml->QueryFloatAttribute("p1_cal", &calc1);
		xml->QueryFloatAttribute("p2_cal", &calc2);

		scale	= (calc2 - calc1)/(raw2 - raw1);
		offset 	= calc1-raw1*scale;
		
		pot->setScale(scale);
		pot->setOffset(offset);
		
		pot->setMinimumRawValue(min_raw);
		pot->setMaximumRawValue(max_raw);
	}

	return pot;
}

/*******************************************************************************
 *
 * Create a gyro
 * 
 * The XML element must have the format of
 *  
 *  <gyro [module="1"] [port="1"] [sensitivity="0.007"] />
 *  
 *  Where:	module	the module (or card) to which this speed controller is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 1 will be used
 *  				
 *  		port	the port to which this speed controller is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *  				
 *  		sensitivity	the sensitivity that will be applied to the raw gyro value
 *  				
 ******************************************************************************/
RGyro *XmlRobotUtil::createGyro(tinyxml2::XMLElement* xml)
{
	RGyro *gyro;
	
#if defined (FRC_CRIO)
	int module	= xml->IntAttribute("module");
	if ((module < 1) || (module > 1))
	{
		Advisory::pinfo("    using default \"module\" of 1");
		module = 1;
	}
#endif
	
	int port	= xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		Advisory::pinfo("    using default \"port\" of 1");
		port = 1;
	}
	
#if defined (FRC_CRIO)
	Advisory::pinfo("    creating gyro on module %d, port %d", module, port);
	gyro = new RGyro(module, port);
#else
	Advisory::pinfo("    creating gyro on port %d", port);
	gyro = new RGyro(port);
#endif
	
	if (gyro != NULL)
	{
		float sensitivity = xml->FloatAttribute("sensitivity");
		if (sensitivity == 0.0)
		{
			Advisory::pinfo("    using default \"sensitivity\" of 0.007");
			gyro->SetSensitivity(0.007);
		}
		else
		{
			gyro->SetSensitivity(sensitivity);
		}
	}

	return gyro;
}

/*******************************************************************************
 *
 * Create a RCounter
 * 
 * The XML element must have the format of
 *  
 *  <counter [module="1"] [port="3"] [count_per_rev="8"] 
 *  	[filter_coeff="0.9"] [...] >
 *  
 *  Where:	module	the module (or card) to which this speed controller is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 1 will be used
 *  				
 *  		port	the port to which this speed controller is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *  				
 *  		count_per_rev	how many times the counter will increment is 
 *  						a single rotation of the output shaft
 *  						
 *  		filter_coeff	the coefficient that will be used to smooth
 *  						the output
 *  						
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
RCounter *XmlRobotUtil::createCounter(tinyxml2::XMLElement* xml)
{
	RCounter *counter;
	
#if defined (FRC_CRIO)
	int module	= xml->IntAttribute("module");
	if ((module < 1) || (module > 2))
	{
		Advisory::pinfo("    using default \"module\" of 1");
		module = 1;
	}
#endif
	
	int port	= xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		// don't default to 1, 1 must be used by gyros
		Advisory::pinfo("    using default \"port\" of 2");
		port = 2;
	}
	
#if defined (FRC_CRIO)
	Advisory::pinfo("    creating counter on module %d, port %d", module, port);
	counter = new RCounter(module, port);
#else
	Advisory::pinfo("    creating counter on port %d", port);
	counter = new RCounter(port);
#endif
	
	if (counter != NULL)
	{
		int cnt_per_rev		= xml->IntAttribute("count_per_rev");
		float fltr_coeff	= xml->FloatAttribute("filter_coeff");

		if (cnt_per_rev < 1)
		{
			cnt_per_rev = 1;
			Advisory::pinfo("    using default \"count_per_rev\" of 1");
		}
		counter->SetCountPerRev(cnt_per_rev);
		
		if (fltr_coeff == 0.0)
		{
			fltr_coeff = 0.5;
			Advisory::pinfo("    using default \"filter_coeff\" of %f", fltr_coeff);
		}
		counter->SetFilterCoeff(fltr_coeff);
	}

	return counter;
}

/*******************************************************************************
 *
 * Create a pid (SimplePID)
 * 
 * The XML element must have the format of
 *  
 *  <pid [kp="kp"] [ki="ki"] [kd="kd"]
 *  	 [targ_min="targ_min"] [targ_max="targ_max"] [targ_thp="targ_thp"]
 *  	 [cntl_min="cntl_min"] [cntl_max="cntl_max"]
 *  	 [...] >
 *  
 *  Where:	kp		proportional constant
 *  		ki		integral constant
 *  		kd		derivative constant
 *  		
 *  		targ_min	minimum allowed value for the target
 *  		targ_max	maximum allowed value for the target
 *  		targ_thp	threashold percentage change
 *  		
 *  		cntl_min	the minimum control value that should be generated
 *  		cntl_max	the maximum control value that should be generated
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
SimplePID *XmlRobotUtil::createPID(tinyxml2::XMLElement* xml)
{
	SimplePID *pid;
	
	Advisory::pinfo("    creating simple PID");
	pid = new SimplePID();
	
	if (pid != NULL)
	{
		float kp		= xml->FloatAttribute("kp");
		float ki		= xml->FloatAttribute("ki");
		float kd		= xml->FloatAttribute("kd");
		
		float targ_min	= xml->FloatAttribute("targ_min");
		float targ_max	= xml->FloatAttribute("targ_max");
		float targ_thp	= xml->FloatAttribute("targ_thp");

		float cntl_min	= xml->FloatAttribute("cntl_min");
		float cntl_max	= xml->FloatAttribute("cntl_max");

		pid->setPIDConstants(kp, ki, kd);
		if (targ_thp != 0.0)
		{
			pid->setTargetLimits(targ_min, targ_max, targ_thp);
		}
		else
		{
			pid->setTargetLimits(targ_min, targ_max, 0.05);
		}
		
		pid->setControlLimits(cntl_min, cntl_max);
		
		Advisory::pinfo("      PID Kp=%f, Ki=%f, Kd=%f, tmn=%f, tmx=%f, thp=%f, cmn=%f, cmx=%f",
				kp, ki, kd, targ_min, targ_max, targ_thp, cntl_min, cntl_max	);
	}

	return pid;
}

/*******************************************************************************
 *
 * Create a trapezoid velocity control of position (SimpleTrapCntl)
 * 
 * The XML element must have the format of
 *  
 *  <pid [kp="kp"] [ki="ki"] [kd="kd"]
 *  	 [vel_min="targ_min"] [vel_max="targ_max"] [vel_thp="targ_thp"]
 *  	 [accel="accel_val"] [cntl_min="cntl_min"] [cntl_max="cntl_max"]
 *  	 [...] >
 *  
 *  Where:	kp		proportional constant
 *  		ki		integral constant
 *  		kd		derivative constant
 *  		
 *  		vel_min	minimum allowed value for the velocity
 *  		vel_max	maximum allowed value for the velocity
 *  		vel_thp	threashold percentage change
 *  		
 * 			accel	the slope of the velocity curve
 * 
 *  		control_min	the minimum control value that should be generated
 *  		control_max	the maximum control value that should be generated
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
SimpleTrapCntl *XmlRobotUtil::createTrapCntl(tinyxml2::XMLElement* xml)
{
	SimpleTrapCntl *trap;
	
	Advisory::pinfo("    creating simple Trapezoid Velocity Control");
	trap = new SimpleTrapCntl();
	
	if (trap != NULL)
	{
		float kf 			= 0.0;
		float kp 			= 0.0;
		float ki 			= 0.0;
		float kd 			= 0.0;
		float vel_min 		= -1.0;
		float vel_max 		= 1.0;
		float vel_thp 		= 0.2;
		float accel			= 1.0;
		float control_min 	= -1.0;
		float control_max 	= 1.0;

		xml->QueryFloatAttribute("kf", &kf);
		xml->QueryFloatAttribute("kp", &kp);
		xml->QueryFloatAttribute("ki", &ki);
		xml->QueryFloatAttribute("kd", &kd);
		xml->QueryFloatAttribute("vel_min", &vel_min);
		xml->QueryFloatAttribute("vel_max", &vel_max);
		xml->QueryFloatAttribute("vel_thp", &vel_thp);
		xml->QueryFloatAttribute("accel", &accel);
		xml->QueryFloatAttribute("control_min", &control_min);
		xml->QueryFloatAttribute("control_max", &control_max);

		trap->setControlConstants(kf, kp, ki, kd);
		trap->setVelocityLimits(vel_min, vel_max, vel_thp);
		trap->setVelocityLimits(vel_min, vel_max, 0.20);
		trap->setAccelerationLimits(accel);
		trap->setControlValueLimits(control_min, control_max);
		
		Advisory::pinfo("      TRAP Kf=%f, Kp=%f, Ki=%f, Kd=%f, tmn=%f, tmx=%f, acc= %f, thp=%f, cmn=%f, cmx=%f",
				kf, kp, ki, kd, vel_min, vel_max, accel, vel_thp, control_min, control_max);
	}

	return trap;
}

/*******************************************************************************
 *
 * Create a trapezoid velocity control of position (SimpleTrapCntl)
 * 
 * The XML element must have the format of
 *  
 *  <pid [kp="kp"] [ki="ki"] [kd="kd"]
 *  	 [vel_min="targ_min"] [vel_max="targ_max"] [vel_thp="targ_thp"]
 *  	 [accel="accel_val"] [cntl_min="cntl_min"] [cntl_max="cntl_max"]
 *  	 [...] >
 *  
 *  Where:	kp		proportional constant
 *  		ki		integral constant
 *  		kd		dirivitive constant
 *  		
 *  		vel_min	minimum allowed value for the velocity
 *  		vel_max	maximum allowed value for the velocity
 *  		vel_thp	threashold perentage change
 *  		
 * 			accel	the slope of the velocity curve
 * 
 *  		cntl_min	the minimum control value that should be generated
 *  		cntl_max	the maximum control value that should be generated
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
TrapezoidalProfile *XmlRobotUtil::createTrapezoid(tinyxml2::XMLElement* xml)
{
	TrapezoidalProfile *trap;
	
	Advisory::pinfo("    creating trapezoidal profile");
	trap = new TrapezoidalProfile();
	
	if (trap != NULL)
	{
		float max_vel		= xml->FloatAttribute("max_vel");
		float percent_accel		= xml->FloatAttribute("percent_accel");
		float delta_time		= xml->FloatAttribute("delta_time");

		trap->setMaxVelocity(max_vel);
		trap->setPercentAcc(percent_accel);
		trap->setDeltaTime(delta_time);

		Advisory::pinfo("      TRAP max_v=%f %% accel=%f dt=%f",max_vel, percent_accel, delta_time);
	}

	return trap;
}

/*******************************************************************************
 *
 * Create a washout filter for a command (WashoutCommand
 * 
 * The XML element must have the format of
 *  
  	<washout name="drive_washout" coefficient="coeff", decay_percent="decay%" />
 *  
 *  Where:	coefficient  decay coefficient (dependent on rate)
 *  		decay_percent is the amount to decay as percent of full command
 *  				
 ******************************************************************************/
WashoutCommand *XmlRobotUtil::createWashout(tinyxml2::XMLElement* xml)
{
	WashoutCommand *wash;
	
	Advisory::pinfo("    creating washout commander");
	wash = new WashoutCommand();
	
	if (wash != NULL)
	{
		float coeff		= xml->FloatAttribute("coefficient");
		float percent_decay		= xml->FloatAttribute("decay_percent");

		wash->setCoefficient(coeff);
		wash->setDecayPercent(percent_decay);

		Advisory::pinfo("      WASHOUT coeff=%f %% decay=%f",coeff, percent_decay);
	}

	return wash;
}

/*******************************************************************************
 *
 * Create an instance of a Relay from the given XML element.
 * 
 * The XML element must have the format of
 *  
 *  <Relay [module="module"] [port="port"] [...] >
 *  
 *  Where:	module	the module (or card) to which this Relay is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 1 will be used (FRC_CRIO Only)
 *  		port	the port to which this Relay is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
Relay *XmlRobotUtil::createRelay(tinyxml2::XMLElement* xml)
{
#if defined (FRC_CRIO)
	int module			= xml->IntAttribute("module");
	if ((module < 1) || (module > 2))
	{
		Advisory::pinfo("    using default \"module\" of 1");
		module = 1;
	}
#endif
	
	int port			= xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		Advisory::pinfo("    using default \"port\" of 1");
		port = 1;
	}


#if defined (FRC_CRIO)
	Advisory::pinfo("    creating relay on module %d, port %d", module, port);
	return new Relay(module, port);
#else
	Advisory::pinfo("    creating relay on port %d", port);
	return new Relay(port);
#endif
}

/*******************************************************************************
 *
 * Create an instance of a Servo from the given XML element.
 * 
 * The XML element must have the format of
 *  
 *  <Servo [module="module"] [port="port"] [...] >
 *  
 *  Where:	module	the module (or card) to which this Servo is 
 *  				connected, can be a vaule of 1 or 2, if a module is not 
 *  				specified, the default value of 1 will be used (FRC_CRIO only)
 *  				
 *  		port	the port to which this Servo is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
Servo *XmlRobotUtil::createServo(tinyxml2::XMLElement* xml)
{
#if defined (FRC_CRIO)
	int module			= xml->IntAttribute("module");
	if ((module < 1) || (module > 2))
	{
		Advisory::pinfo("    using default \"module\" of 1");
		module = 1;
	}
#endif
	
	int port			= xml->IntAttribute("port");
	if ((port < 0) || (port > 20))
	{
		Advisory::pinfo("    using default \"port\" of 1");
		port = 1;
	}

#if defined (FRC_CRIO)
	Advisory::pinfo("    creating servo on module %d, port %d", module, port);
	return new Servo(module, port);
#else
	Advisory::pinfo("    creating servo on port %d", port);
	return new Servo(port);
#endif
}

/*******************************************************************************
 *
 * Create an instance of a Digital Input from the given XML element.
 *
 * @param	xml	the xml that defines the digital input.
 * 
 * The XML element must have the format of
 *  
 *  <digital_input [port="port"] [normally_open="true"] [...] >
 *  
 *  Where:	port	the port to which this DigitalInput is connected,
 *  				can be a value from 0 to ??, if not specified, the
 *  				default value of 1 is used.
 *  				
 *  		normally_open	true if the connected switch is normally open
 *  						false if it is normally closed
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="upper_limit"
 *  				
 *  @return	a pointer to a digital input object, the calling class is
 *  		responsible for deleting the returned object.
 *
 ******************************************************************************/
RDigitalInput *XmlRobotUtil::createDigitalInput(tinyxml2::XMLElement* a_xml)
{
	int port = 1;
	bool normally_open = true;

	a_xml->QueryIntAttribute("port", &port);
	a_xml->QueryBoolAttribute("normally_open", &normally_open);
	
	if (port < 0)
	{
		Advisory::pinfo("    using default \"port\"");
		port = 1;
	}

	Advisory::pinfo("    creating %s digital input on port %d", normally_open ? "normally open" : "normally closed", port);

	RDigitalInput *rdi = new RDigitalInput(port);
	if (nullptr != rdi)
	{
		rdi->setMode(normally_open ? RDigitalInput::INPUT_NORMALLY_OPEN : RDigitalInput::INPUT_NORMALLY_CLOSED);
	}
	else
	{
		Advisory::pinfo("ERROR: failed to create digital input");
	}

	return rdi;
}


/*******************************************************************************
 *
 ******************************************************************************/
DigitalOutput *XmlRobotUtil::createDigitalOutput(tinyxml2::XMLElement* xml)
{
	int port = 1;
	xml->QueryIntAttribute("port", &port);

	if (port < 0)
	{
		Advisory::pinfo("    using default \"port\"");
		port = 1;
	}

	Advisory::pinfo("    creating digital output on port %d", port);

	DigitalOutput *rdo = new DigitalOutput(port);
	if (nullptr == rdo)
	{
		Advisory::pinfo("ERROR: failed to create digital input");
	}

	return rdo;
}

/*******************************************************************************
 *
 * Create an instance of a Scaled Analog from the given XML element.
 *
 * @param	xml	the xml that defines the scaled analog.
 *
 * The XML element must have the format of
 *
 *  <scaled_analog [module="module"] [port="port"]
 *  	[p1_raw="1.0"] [p1_cal="0.0"] [p2_raw="3.0"] [p2_cal="120.0"]
 *    [min_raw="min_raw"] [max_raw="max_raw"] [...] >
 *
 *  Where:	module	the module (or card) to which this speed controller is
 *  				connected, can be a vaule of 1 or 2, if a module is not
 *  				specified, the default value of 1 will be used (FRC_CRIO Only)
 *
 *  		port	the port to which this speed controller is conncected,
 *  				can be a value from 1 to 10, if not specified, the
 *  				default value of 1 is used.
 *
 *  		min_raw	the minimum raw value that will be considered as
 *  				valid, if the raw value is less than this, isReady()
 *  				will return false.  This is to help detect failed
 *  				sensors.
 *
 *  		max_raw	the maximum raw value that will be considered as
 *  				valid, if the raw value is greater than this, isReady()
 *  				will return false.  This is to help detect failed
 *  				sensors.
 *
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"

 *
 *  @return	a pointer to a digital input object, the calling class is
 *  		responsible for deleting the returned object.
 *
 ******************************************************************************/
ScaledAnalog *XmlRobotUtil::createScaledAnalog(tinyxml2::XMLElement* xml)
{
	ScaledAnalog *sa;

	int port	= 0;

	xml->QueryIntAttribute("port", &port);

	Advisory::pinfo("    creating Scaled Analog on port %d", port);
	sa = new ScaledAnalog(port);

	if (sa != NULL)
	{
		float p1_raw = 0.0;
		float p2_raw = 5.0;
		float p1_cal = 0.0;
		float p2_cal = 5.0;

		float scale = 1.0;
		float offset = 0.0;

		float min_raw = 0.05;
		float max_raw = 4.95;

		xml->QueryFloatAttribute("min_raw", &min_raw);
		xml->QueryFloatAttribute("max_raw", &max_raw);

		xml->QueryFloatAttribute("p1_raw", &p1_raw);
		xml->QueryFloatAttribute("p2_raw", &p2_raw);
		xml->QueryFloatAttribute("p1_cal", &p1_cal);
		xml->QueryFloatAttribute("p2_cal", &p2_cal);

		scale	= (p2_cal - p1_cal)/(p2_raw - p1_raw);
		offset 	= p1_cal-p1_raw*scale;

		sa->setScale(scale);
		sa->setOffset(offset);

		sa->setMinimumRawValue(min_raw);
		sa->setMaximumRawValue(max_raw);
	}

	return sa;
}

/*******************************************************************************
 *
 * Get the period attribute and do limit checking
 * 
 * The XML element must have the format of
 *  
 *  <control [period="period"] [...] >
 *  
 *  Where:	period		controls period in seconds
 *  	
 *  		...		other options may be required based on use, a common
 *  				option would be something like name="shoot"
 *  				
 ******************************************************************************/
double XmlRobotUtil::getPeriod(tinyxml2::XMLElement *xml)
{
	double period = xml->DoubleAttribute("period");

	// check for valid period
	if (period < 0.01)
	{
		period = 0.01;
		Advisory::pinfo("    using minimum period of %5.3f", period);
	}
	else if (period > 10.0)
	{
		period = 10.0;
		Advisory::pinfo("    using maximum period of %5.3f", period);
	}
	
	return period;
}

/*******************************************************************************
 *
 * Get the system priority from the specified attribute and do limit and 
 * error checking
 * 
 * The XML element must have the format of
 *  
 *  <control [priority="priority"] [...] >
 *
 *  Where:	priority is the controls thread priority 
 *  		not specified means use the default priority, 
 *          a value of  0 means use the default priority, 
 *          a value of -1 means use low priority,
 *          a value of -2 means use lower priority,
 *          a value of -3 or less means use lowest priority,
 *          a value of  1 means use high priority,
 *          a value of  2 means use higher priority,
 *          a value of  3 or more means use highest priority
 * 
 ******************************************************************************/
gsi::Thread::ThreadPriority XmlRobotUtil::getPriority(tinyxml2::XMLElement *xml)
{
	int priority = xml->IntAttribute("priority");
	
	if (priority == 0)
	{
		 return gsi::Thread::PRIORITY_DEFAULT; 
	}
	else if (priority <= -3)
	{
		 return gsi::Thread::PRIORITY_LOWEST; 
	}
	else if (priority == -2)
	{
		 return gsi::Thread::PRIORITY_LOWER; 
	}
	else if (priority == -1)
	{
		 return gsi::Thread::PRIORITY_LOW; 
	}
	else if (priority == 1)
	{
		 return gsi::Thread::PRIORITY_HIGH; 
	}
	else if (priority == 2)
	{
		 return gsi::Thread::PRIORITY_HIGHER; 
	}
	else
	{
		 return gsi::Thread::PRIORITY_HIGHEST; 
	}
}

/*******************************************************************************
 *
 * Create a gyro class, either SPI or Analog
 *
 ******************************************************************************/
Gyro *XmlRobotUtil::createGyroAny(tinyxml2::XMLElement* xml)
{

	const char *type 	= xml->Attribute("type");  // could be SPI or analog

	if(0 == strcmp(type, "analog"))  // create analog gyro
	{
		return(createGyro(xml));
	}
	else if(0 == strcmp(type, "SPI"))  // create SPI gyro
	{
		return(createSpiGyro(xml));
	}
	else
	{
		Advisory::pinfo("unsupported gyro type %s", type);
	}
	return(nullptr);

}

RAnalogIrSensor *XmlRobotUtil::createAnalogIrSensor(tinyxml2::XMLElement* xml)
{
	int channel = xml->IntAttribute("channel");
	int threshold = DEFAULT_THRESHOLD_VALUE;
	int cycles = DEFAULT_LATCH_CYLES;
	RAnalogIrSensor::ThresholdType threshold_type = RAnalogIrSensor::kLessThan;
	const char *type 	= xml->Attribute("threshold_type");

	RAnalogIrSensor *ir = nullptr;

	if(channel >= 0)
	{
		ir = new RAnalogIrSensor(channel);
	}
	xml->QueryIntAttribute("latch_cycles", &cycles);
	xml->QueryIntAttribute("threshold", &threshold);

	if(nullptr != type)
	{
		if(0 == strcmp(type, "greater_than"))
		{
			threshold_type = RAnalogIrSensor::kGreaterThan;
		}
	}
	if(nullptr != ir)
	{
		ir->setLatchCycles(cycles);
		ir->setThreshold(threshold);
		ir->setThresholdType(threshold_type);
	}
	return(ir);

}
