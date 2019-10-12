/*******************************************************************************
*
* File: BuildVersion.h
*
* Written by:
* 	The Robonauts
* 	FRC Team 118
* 	NASA, Johnson Space Center
* 	Clear Creek Independent School District
*
******************************************************************************/
#pragma once

#include <stdint.h>
#include <string>

namespace gsu
{

/*******************************************************************************
 *
 * This class can be used to share Build Version information with any class
 * that needs it. Because it is a library class, the version information must
 * be set externally so it represents the applications version not
 * the libraries version.
 *
 * Usage: 
 *     in main(), BuildVersion::setVersion(0,0,0,0);
 *     anyplace,  BuildVersion::getVersionStr();
 *
 ****************************************************************************/
class BuildVersion
{
	public:
		static void setVersion(uint16_t major = 0, uint16_t minor = 0, uint16_t patch = 0, uint16_t build = 0);

		static uint16_t getMajor(void);
		static uint16_t getMinor(void);
		static uint16_t getPatch(void);
		static uint16_t getBuild(void);

		static std::string getVersionStr(void);

	private:
		static uint16_t c_major;
		static uint16_t c_minor;
		static uint16_t c_patch;
		static uint16_t c_build;
};

}
