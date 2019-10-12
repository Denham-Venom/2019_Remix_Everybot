/*******************************************************************************
 *
 * File: PathUtil.h
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

#include <string>

namespace gsu
{

/*******************************************************************************
 *
 * This class provides a platform independent interface to a path information.
 *
 ******************************************************************************/
class PathUtil
{
	public:
		static const std::string BIN_PATH_ID;
		static const std::string DATA_PATH_ID;

		static void init(int argc, char* argv[], std::string vendor_name, std::string application_name,
				std::string appdata_default, std::string appdata_env_var="APPDATA");

		static void initPath(std::string path_id, std::string dir_name, std::string env_name = "",
				std::string cmd_switch = "", int argc = 0, char* argv[] = nullptr);

		static std::string getVendorName(void);
		static std::string getApplicationName(void);

		static void setPath(std::string path_id, std::string path);
		static std::string getPath(std::string path_id, std::string filename = "");

	private:
		static std::string mApplicationName;
		static std::string mVendorName;
};

} // namespace gsu
