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
#include "gsi/Path.h"

#if defined(_WINDOWS)
#include <windows.h>
#include <io.h>
#include <stdlib.h>
#include <psapi.h>
#include <sstream>
#include <ostream>
#include <cstdlib>
#include <iostream>

using std::getenv;

#else
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#endif

using namespace std;

namespace gsi
{

/**
* Allocation of static variables
*/
#if defined(_WINDOWS)
char Path::FILE_SEPARATOR = '\\';
char Path::FILE_SEPARATORS[] = { '/', '\\' };
uint8_t Path::FILE_SEPARATORS_COUNT = 2;

#else
char Path::FILE_SEPARATOR = '/';
char Path::FILE_SEPARATORS[] = { '/' };
uint8_t Path::FILE_SEPARATORS_COUNT = 1;

// #define EXE_LINK "/proc/curproc/file" // FreeBSD
// #define EXE_LINK "/proc/self/path/a.out" // Solaris
#define EXE_LINK "/proc/self/exe"

#endif

/*******************************************************************************
 *
 * @param	arg0, the first argument passed to the main method of this
 * 			application/executable/process, if other means cannot be used to
 * 			get the filename, this argument will be used.
 *
 *	@return	the fully qualified name of the file that contains the
 *			application/executable/process that is calling this method,
 *			if the filename cannot be determined, an empty string is returned
 *
 ******************************************************************************/
std::string Path::getExecutableFile(const char* arg0)
{
#if defined(_WINDOWS)
	char temp[MAX_PATH];
	DWORD size = GetModuleFileNameA(nullptr, temp, MAX_PATH);
	if (size > 0)
	{
		return std::string(temp);
	}
	return "";
#else
	char temp[PATH_MAX];
	memset(temp, 0, PATH_MAX);

	size_t read_size = readlink(EXE_LINK, temp, 1024); // Linux
	if ((read_size > 0) && (read_size < PATH_MAX-1))
	{
		return string(temp);
	}

	// use argv[0], not as reliable
	if (arg0 != nullptr)
	{
		return string(realpath(arg0, temp));
	}
	return "";
#endif
}

/*******************************************************************************
 *
 * Get the name of the Executable, this is the filename without the leading
 * path or the file extension.
 *
 * @param	arg0, the first argument passed to the main method of this
 * 			application/executable/process, if other means cannot be used to
 * 			get the filename, this argument will be used.
 *
 * @return 	the name of the executable
 *
 ******************************************************************************/
std::string Path::getExecutableName(const char* arg0)
{
	string file = getExecutableFile(arg0);

	size_t pos = file.find_last_of(FILE_SEPARATORS);
	if (pos != string::npos)
	{
		file = file.substr(pos + 1);
	}

	pos = file.find_last_of('.');
	if (pos != string::npos)
	{
		file.resize(pos);
	}

	return file;
}

/*******************************************************************************
 *
 * Get the path/folder/directory that the executable image is in.
 *
 * @param	arg0, the first argument passed to the main method of this
 * 			application/executable/process, if other means cannot be used to
 * 			get the filename, this argument will be used.
 *
 * @return the path to the executable image
 *
 ******************************************************************************/
std::string Path::getExecutablePath(const char* arg0)
{
	string file = getExecutableFile(arg0);

	size_t pos = file.find_last_of(FILE_SEPARATORS);
	if (pos != string::npos)
	{
		file.resize(pos);
	}

	return file;
}


/*******************************************************************************
*
******************************************************************************/
std::string Path::appendFileSeparator(std::string path)
{
	if (path.length() == 0)
	{
		return to_string(Path::FILE_SEPARATOR);
	}

	char last_char = path[path.length() - 1];

	for (int i = 0; i < FILE_SEPARATORS_COUNT; i++)
	{
		if (FILE_SEPARATORS[i] == last_char)
		{
			return path;
		}
	}

	return path + Path::FILE_SEPARATOR;
}

/*******************************************************************************
 *
 * @param	path	the path to check
 *
 * @returns true if the provided path/folder/directory exists and is not the
 * 			name of a file
 *
 ******************************************************************************/
bool Path::pathExists(std::string path)
{
#if defined(_WINDOWS)
	DWORD dwAttrib = GetFileAttributes(path.c_str());

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat fileStat;

	return (stat(path.c_str(), &fileStat) >= 0);
#endif
}

/*******************************************************************************
 *
 * @param	file	the fully qualified name of a file to check
 *
 * @return true if the specified file exists
 *
 ******************************************************************************/
bool Path::fileExists(std::string file)
{
#if defined(_WINDOWS)
	DWORD dwAttrib = GetFileAttributes(file.c_str());

	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
	struct stat fileStat;

	return (stat(file.c_str(), &fileStat) >= 0);
#endif
}

/*******************************************************************************
 *
 * Create the specified path, this means create all levels/directories/folders
 * needed to make sure the full path exists.
 *
 * @return 	true if the path exists when this method returns, false if for
 * 			any reason the path could not be created
 *
 ******************************************************************************/
bool Path::createPath(std::string path)
{
	if (pathExists(path))
	{
		return true;
	}
	else
	{
		size_t pos = path.find_last_of(FILE_SEPARATORS);
		if (pos != std::string::npos)
		{
			if (createPath(path.substr(0, pos))) // make sure the parent exists
			{
#if defined(_WINDOWS)
				CreateDirectory(path.c_str(), NULL);
#else
				mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
#endif
				return pathExists(path);
			}
		}
	}

	return false;
}

/*******************************************************************************
 *
 * This method does not really belong here, but we do not have another
 * class to put it in at this time
 *
 ******************************************************************************/
std::string Path::getEnvVariable(std::string var)
{
	char * val;
	val = getenv(var.c_str());
	if (val != NULL)
	{
		return string(val);
	}

	return "";
}

/*******************************************************************************
 *
 * This method does not really belong here, but we do not have another
 * class to put it in at this time
 *
 ******************************************************************************/
void Path::setEnvVariable(std::string /*var*/, std::string /*val*/)
{
	//	setenv(var.c_str(), val.c_str(), true);
}

} // namespace gsi
