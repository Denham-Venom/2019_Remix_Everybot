/*******************************************************************************
 *
 * File: Config.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <stdlib.h>

#include "gsu/Config.h"

using namespace std;

namespace gsu
{
/*******************************************************************************
 *
 *
 ******************************************************************************/
void Config::init(std::string filename)
{
    m_file.loadFile(filename);
}

/*******************************************************************************
 *
 * Removes all parameters from this object
 *
 ******************************************************************************/
void Config::clear(void)
{
    m_file.clear();
}

/*******************************************************************************
 *
 * Load the specified file into this class.
 *
 * When a new file is loaded, new parameters are added to this instance. Any 
 * parameters that are in the new file and are already in this instance of 
 * this class will be updated to the values in the new file.  This allows 
 * multiple files to be read, duplicate parameters will take on the values 
 * from the last file read.
 *
 * @param	filename	the fully qualified name of the file to load.
 *
 ******************************************************************************/
void Config::loadFile(string filename)
{
    m_file.loadFile(filename);
}

/*******************************************************************************
 *
 * Save the contents of this class into the optionally specified file.
 *
 * If a filename is not specified, the last provided filename (to the constructor,
 * LoadFile() method, or SaveFile() method) will be used. If no filename was ever
 * provided, the file will be saved as parameters.xml in the current directory.
 *
 * @param	filename	the fully qualified name of the file to load.
 *
 ******************************************************************************/
void Config::saveFile(string filename)
{
    m_file.saveFile(filename);
}

/*******************************************************************************
 *
 * Can be used to determine if the named parameter is has
 * a value in this instance.
 *
 * @param	name		the name of the parameter
 *
 * @return true if the parameter is in the map.
 *
 ******************************************************************************/
bool Config::contains(string name)
{
    return m_file.contains(name);
}

/*******************************************************************************
 *
 * return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	true if the map value starts with the letter 't' or 'T',
 *			false if the map value starts with the letter 'f' or 'F',
 *			default_val for any other condition.
 *
 ******************************************************************************/
bool Config::get(string name, bool default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, bool value)
{ 
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to a
 *			byte, the file value is returned, else the default value
 *			is returned
 *
 ******************************************************************************/
int8_t	Config::get(string name, int8_t default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, int8_t value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to an
 *			unsigned byte, the file value is returned, else the default value
 *			is returned
 *
 ******************************************************************************/
uint8_t	Config::get(string name, uint8_t default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, uint8_t value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to a
 *			short, the file value is returned, else the default value
 *			is returned
 *
 ******************************************************************************/
int16_t	Config::get(string name, int16_t default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, int16_t value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to an
 *			unsigned short, the file value is returned, else the default value
 *			is returned
 *
 ******************************************************************************/
uint16_t Config::get(string name, uint16_t default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, uint16_t value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to an
 *			integer, the file value is returned, else the default value
 *			is returned
 *
 ******************************************************************************/
int32_t	Config::get(string name, int32_t default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, int32_t value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to an
 *			unsigned integer, the file value is returned, else the default value
 *			is returned
 *
 ******************************************************************************/
uint32_t	Config::get(string name, uint32_t default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, uint32_t value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found and can be converted to a
 *			float, the file value is returned, else the default value 
 *			is returned
 *
 ******************************************************************************/
float	Config::get(string name, float default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, float value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param	name		the name of the parameter
 * @param	default_val	the value that will be returned if the specified
 *						name is not in the map
 *
 * @return	if the specified name is found, the file value
 *			is returned, else the default value is returned
 *
 ******************************************************************************/
string Config::get(string name, string default_val)
{
    return m_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Config::put(string name, string value)
{
    m_file.put(name, value);
}

/*******************************************************************************
 *
 * If the specified parameter is found in the map, remove it.
 *
 * @param	name	the name of the parameter
 *
 ******************************************************************************/
void Config::remove(string name)
{
    m_file.remove(name);
}

} // namespace gsu
