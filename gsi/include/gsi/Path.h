/*******************************************************************************
 *
 * File: Path.h
 * 	Generic System Interface for accessing path/directory/folder information
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

namespace gsi
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to a path information.
 *
 ******************************************************************************/
class Path
{
	public:
		static char FILE_SEPARATOR;    // preferred path separator on this system
		static char FILE_SEPARATORS[]; // possible path separators on this system
		static uint8_t FILE_SEPARATORS_COUNT;

		static std::string getExecutableFile(const char* arg0 = nullptr);
		static std::string getExecutableName(const char* arg0 = nullptr);
		static std::string getExecutablePath(const char* arg0 = nullptr);

		static std::string appendFileSeparator(std::string path);

		static bool pathExists(std::string path);
		static bool createPath(std::string path);
		static bool fileExists(std::string path);

		static std::string getEnvVariable(std::string var);
		static void setEnvVariable(std::string var, std::string val);
};

} // namespace gsi
