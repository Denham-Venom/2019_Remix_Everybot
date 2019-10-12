/*******************************************************************************
 *
 * File: Parameter.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <stdlib.h>

#include "gsu/Parameter.h"

using namespace std;

namespace gsu
{
ParameterFile f_file;

/*******************************************************************************
 *
 *
 ******************************************************************************/
void Parameter::init(std::string filename)
{
	f_file.loadFile(filename);
}

/*******************************************************************************
 *
 * Removes all parameters from this object
 *
 ******************************************************************************/
void Parameter::clear(void)
{
	f_file.clear();
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
void Parameter::loadFile(string filename)
{
	f_file.loadFile(filename);
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
void Parameter::saveFile(string filename)
{
	f_file.saveFile(filename);
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
bool Parameter::contains(string name)
{
	return f_file.contains(name);
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
bool Parameter::get(string name, bool default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, bool value)
{ 
	f_file.put(name, value);
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
int8_t	Parameter::get(string name, int8_t default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, int8_t value)
{
	f_file.put(name, value);
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
uint8_t	Parameter::get(string name, uint8_t default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, uint8_t value)
{
	f_file.put(name, value);
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
int16_t	Parameter::get(string name, int16_t default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, int16_t value)
{
	f_file.put(name, value);
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
uint16_t Parameter::get(string name, uint16_t default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, uint16_t value)
{
	f_file.put(name, value);
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
int32_t	Parameter::get(string name, int32_t default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, int32_t value)
{
	f_file.put(name, value);
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
uint32_t	Parameter::get(string name, uint32_t default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, uint32_t value)
{
	f_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param   name        the name of the parameter
 * @param   default_val the value that will be returned if the specified
 *                      name is not in the map
 *
 * @return  if the specified name is found and can be converted to a
 *          float, the file value is returned, else the default value
 *          is returned
 *
 ******************************************************************************/
float   Parameter::get(string name, float default_val)
{
    return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param   name    the name of the parameter
 * @param   value   the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, float value)
{
    f_file.put(name, value);
}

/*******************************************************************************
 *
 * Return a value for the specified parameter
 *
 * @param   name        the name of the parameter
 * @param   default_val the value that will be returned if the specified
 *                      name is not in the map
 *
 * @return  if the specified name is found and can be converted to a
 *          float, the file value is returned, else the default value
 *          is returned
 *
 ******************************************************************************/
double   Parameter::get(string name, double default_val)
{
    return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param   name    the name of the parameter
 * @param   value   the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, double value)
{
    f_file.put(name, value);
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
string Parameter::get(string name, string default_val)
{
	return f_file.get(name, default_val);
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void Parameter::put(string name, string value)
{
	f_file.put(name, value);
}

/*******************************************************************************
 *
 * If the specified parameter is found in the map, remove it.
 *
 * @param	name	the name of the parameter
 *
 ******************************************************************************/
void Parameter::remove(string name)
{
	f_file.remove(name);
}

} // namespace gsu
