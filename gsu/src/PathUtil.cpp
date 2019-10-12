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
#include "gsu/PathUtil.h"

#include <stdio.h>
#include <string.h>
#include <map>
#include <gsi/Path.h>

using namespace std;
using namespace gsi;

namespace gsu
{

/**
* Allocation of static variables
*/

const std::string PathUtil::BIN_PATH_ID = "app_bin";
const std::string PathUtil::DATA_PATH_ID = "app_data";

std::string PathUtil::mVendorName = "UnknownVendor";
std::string PathUtil::mApplicationName = "UnknownApp";

map<std::string, std::string> gPaths;

/*******************************************************************************
 *
 * The Init method will save the vendor and application names.
 *
 * Then it will initialize the BIN_PATH_ID path based on the location of the
 * executable.
 *
 * Finally the DATA_PATH_ID path will be initialized based on the command line
 * arguments, environment variables, and provided default app data path as
 * follows:
 *    - If the switch of '-appdata' is found in the command line arguments the
 *      next argument will be used as the app data path.
 *    - Else if the APPDATA environment variable is found, it will be used as
 *      the base of the app data path, the vendor and application names will
 *      be appended to form the entire path -- %APPDATA%/<vendor>/<application>
 *    - Else the default path will be used. If the default path starts with a
 *      '.' or a '..', it will be treated as relative to the BIN_PATH_ID path.
 *
 * After calling Init(), the InitPath and/or SetPath methods can be called to
 * define other paths used by the application. Then any software can call
 * GetPath to access appliation defined paths.
 *
 * Path::Init(argc, argv, "MyVendor", Path::GetExecutableName(), "..");
 * Path::Init(0, nullptr, "", "MyApp", "/");
 *
 * @see InitPath, SetPath, and GetPath
 *
 ******************************************************************************/
void PathUtil::init(int argc, char* argv[], std::string vendor_name,
	std::string application_name, std::string appdata_default, std::string appdata_env_var)
{
	mVendorName = vendor_name;
	mApplicationName = application_name;

	if ((argc > 0) && (argv != nullptr))
	{
		gPaths[BIN_PATH_ID] = Path::getExecutablePath(argv[0]);
	}
	else
	{
		gPaths[BIN_PATH_ID] = Path::getExecutablePath();
	}

	// setup AppData
	int arg = 0;
	while (arg < argc)
	{
		if ((strcmp(argv[arg], "-appdata") == 0) && (arg + 1 < argc))
		{
			gPaths[DATA_PATH_ID] = string(argv[arg + 1]);
			break;
		}
		arg++;
	}

	if (gPaths[DATA_PATH_ID].length() <= 0)
	{
		std::string temp = Path::getEnvVariable(appdata_env_var);
		if (temp.length() > 0)
		{
			if (mVendorName.length() > 0)
			{
				temp = temp + Path::FILE_SEPARATOR + mVendorName;
			}

			if (mApplicationName.length() > 0)
			{
				temp = temp + Path::FILE_SEPARATOR + mApplicationName;
			}
			gPaths[DATA_PATH_ID] = temp;
		}
	}

	if ((gPaths[DATA_PATH_ID].length() <= 0) && (appdata_default.length() > 0))
	{
		if (appdata_default[0] == '.')
		{
			gPaths[DATA_PATH_ID] = getPath(BIN_PATH_ID, appdata_default);
		}
		else
		{
			gPaths[DATA_PATH_ID] = appdata_default;
		}
	}

	if (gPaths[DATA_PATH_ID].length() <= 0)
	{
		gPaths[DATA_PATH_ID] = getPath(BIN_PATH_ID, "..");
	}

	Path::createPath(gPaths[DATA_PATH_ID]);
}

/*******************************************************************************
 *
 * The InitPath() method will initialize an application defined path that
 * can be used anywhere in the rest of the applications software.
 *
 * Any previous definition of this path will be overridden by this call.
 *
 * The path will be initialized based on the command line arguments, environment
 * variables, and provided information as follows:
 *    - If the cmd_switch is found in the command line arguments the next
 *      argument will be used as the path.
 *    - Else if the env_name environment variable is found, it will be used as
 *      the path.
 *    - Else the provided dir_name in the DATA_PATH_ID path will be used, this
 *      directory will be created if needed.
 *
 *
 * #define CONFIG_PATH_ID "_config"
 * #define IMAGE_PATH_ID  "unique_images"
 * #define LOG_PATH_ID    "something_unique"
 *
 * -- config path defaults to [appdata]/config, can be set command line or as environment variable
 * InitPath(CONFIG_PATH_ID, "config", "FOO_CONFIG_DIR", "-config_path", argc, argv);
 *
 * -- image path defaults to [appdata]/images, can be set as environment variable, but not command line
 * InitPath(IMAGE_PATH_ID, "images", "BAR_CONFIG_DIR");
 *
 * -- log path is always [appdata]/logs
 * InitPath(LOG_PATH_ID, "logs")
 *
 ******************************************************************************/
void PathUtil::initPath(std::string path_id, std::string dir_name, std::string env_name,
	std::string cmd_switch, int argc, char* argv[])
{
	gPaths[path_id] = "";

	int arg = 0;
	while (arg < argc)
	{
		if ((strcmp(argv[arg], cmd_switch.c_str()) == 0) && (arg + 1 < argc))
		{
			gPaths[path_id] = string(argv[arg + 1]);
			return;
		}
		arg++;
	}

	gPaths[path_id] = Path::getEnvVariable(env_name);
	if (gPaths[path_id].length() > 0)
	{
		return;
	}

	std::string temp = getPath(DATA_PATH_ID);
	if (temp.length() > 0)
	{
		gPaths[path_id] = temp + dir_name;
		Path::createPath(gPaths[path_id]);
	}
}

/*******************************************************************************
 *
 * This method can be used to set a hard coded path, or a path that is
 * derived by application specific code. Once the path is set, all other
 * parts of the software can use the path.
 *
 * #define CONFIG_PATH_ID "_config"
 * #define IMAGE_PATH_ID  "unique_images"
 * #define LOG_PATH_ID    "something_unique"
 *
 * -- config path is always /config
 * SetPath(CONFIG_PATH_ID, "/config");
 *
 * -- image is always [appdata]/images
 * SetPath(IMAGE_PATH_ID, Path::GetPath(PathUtil::DATA_PATH_ID, "images"));
 *
 * -- log path is always [bin]/../logs/[application]
 * SetPath(LOG_PATH_ID, Path::GetPath(PathUtil::BIN_PATH_ID) + ".." + Path::FILE_SEPARATOR +
 *    "logs" + Path::FILE_SEPARATOR + Path::GetApplicationName());
 *
 ******************************************************************************/
void PathUtil::setPath(std::string path_id, std::string path)
{
	gPaths[path_id] = path;
}

/*******************************************************************************
 *
 * Get a named path. If the optional filename is provided, the filename will
 * be appended to the named path. If the named path is not known, the current
 * working directory will be used.
 *
 ******************************************************************************/
std::string PathUtil::getPath(std::string path_id, std::string filename)
{
	if (gPaths.find(path_id) != gPaths.end())
	{
		return gPaths[path_id] + gsi::Path::FILE_SEPARATOR + filename;
	}

	return "." + gsi::Path::FILE_SEPARATOR + filename;
}

/*******************************************************************************
 *
 * Get the Vendor name as set by the Init() method.
 *
 ******************************************************************************/
std::string PathUtil::getVendorName(void)
{
	return mVendorName;
}

/*******************************************************************************
 *
 * Get the application name as set by the Init() method.
 *
 ******************************************************************************/
std::string PathUtil::getApplicationName(void)
{
	return mApplicationName;
}

} // namespace gsu
