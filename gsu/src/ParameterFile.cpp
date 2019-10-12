/*******************************************************************************
 *
 * File: ParameterFile.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include <stdlib.h>

#include "gsu/ParameterFile.h"
#include "gsu/tinyxml2.h"

#include <limits>

using namespace std;
using namespace tinyxml2;

namespace gsu
{

/*******************************************************************************
 *
 * Create an uninitialized instance of this class. 
 *
 * This default constructor will create an instance of the 
 * ParameterFile class that does not contain any parameters.
 *
 ******************************************************************************/
ParameterFile::ParameterFile(void)
{
}

/*******************************************************************************
 *
 * Initialize this class by reading the named file.
 *
 * If the named file exists and can be read, it will be parsed for
 * parameter elements. If the file can not be read no parameters
 * will be added to this instance.
 *
 * @param	filename	the fully qualified name of the file to load.
 *
 ******************************************************************************/
ParameterFile::ParameterFile(string filename)
{
	loadFile(filename);
}

/*******************************************************************************
 *
 * Removes all parameters from this object
 *
 ******************************************************************************/
void ParameterFile::clear(void)
{
	mParameters.clear();
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
void ParameterFile::loadFile(string filename)
{
	mFilename = filename;

	XMLDocument domDoc;
	domDoc.LoadFile(filename.c_str());

	if (domDoc.ErrorID() == XML_NO_ERROR)
	{
		XMLElement *root = domDoc.FirstChildElement("parameters");
			
		if (root != nullptr)
		{
			XMLElement *child = root->FirstChildElement("param");
			while (child != nullptr)
			{
				if ((child->Attribute("name") != nullptr) && (child->Attribute("value") != nullptr))
				{
					mParameters[child->Attribute("name")] = child->Attribute("value");
				}
				child = child->NextSiblingElement("param");
			}
		}
	}
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
void ParameterFile::saveFile(string filename)
{
	if (filename.length() > 0)
	{
		mFilename = filename;
	}

        if (mFilename.length() <= 0)
	{
		mFilename = "parameters.txt";
	}

	XMLDocument domDoc;

	XMLElement *root = domDoc.NewElement("parameters");
	domDoc.InsertFirstChild(root);

	map<string, string>::iterator ittr;
	for(ittr = mParameters.begin(); ittr != mParameters.end(); ittr++) 
	{
		XMLElement *pnode = domDoc.NewElement("param");
		pnode->SetAttribute("name", ittr->first.c_str());
		pnode->SetAttribute("value", ittr->second.c_str());
		root->InsertEndChild(pnode);
	}

	domDoc.SaveFile(mFilename.c_str());
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
bool ParameterFile::contains(string name)
{
	return (mParameters.find(name) == mParameters.end());
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
bool ParameterFile::get(string name, bool default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		if ((ittr->second[0] == 't') || (ittr->second[0] == 'T'))
		{
			return true;
		}
		else if ((ittr->second[0] == 'f') || (ittr->second[0] == 'F'))
		{
			return false;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, bool value)
{ 
	mParameters[name] = string(value?"true":"false"); 
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
int8_t	ParameterFile::get(string name, int8_t default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		try
		{
			long val = stoul(ittr->second);

			if ((val >= numeric_limits<int8_t>::min()) && (val <= numeric_limits<int8_t>::max()))
			{
				return (int8_t)val;
			}
		}
		catch (...)
		{
			return default_val;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, int8_t value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] = to_string((long long)value);
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
uint8_t	ParameterFile::get(string name, uint8_t default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		try
		{
			unsigned long val = stoul(ittr->second);

			if ((val >= numeric_limits<uint8_t>::min()) && (val <= numeric_limits<uint8_t>::max()))
			{
				return (uint8_t)val;
			}
		}
		catch (...)
		{
			return default_val;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, uint8_t value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] = to_string((unsigned long long)value);
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
int16_t	ParameterFile::get(string name, int16_t default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		try
		{
			long val = stol(ittr->second);

			if ((val >= numeric_limits<int16_t>::min()) && (val <= numeric_limits<int16_t>::max()))
			{
				return (int16_t)val;
			}
		}
		catch (...)
		{
			return default_val;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, int16_t value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] = to_string((long long)value);
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
uint16_t ParameterFile::get(string name, uint16_t default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		try
		{
			unsigned long val = stoul(ittr->second);

			if ((val >= numeric_limits<uint16_t>::min()) && (val <= numeric_limits<uint16_t>::max()))
			{
				return (uint16_t)val;
			}
		}
		catch (...)
		{
			return default_val;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, uint16_t value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] =to_string((unsigned long long)value);
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
int32_t	ParameterFile::get(string name, int32_t default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		try
		{
			long val = stoul(ittr->second);

			if ((val >= numeric_limits<int32_t>::min()) && (val <= numeric_limits<int32_t>::max()))
			{
				return (int32_t)val;
			}
		}
		catch (...)
		{
			return default_val;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, int32_t value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] = to_string((long long)value);
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
uint32_t	ParameterFile::get(string name, uint32_t default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		try
		{
			unsigned long val = stoul(ittr->second);

			if ((val >= numeric_limits<uint32_t>::min()) && (val <= numeric_limits<uint32_t>::max()))
			{
				return (uint32_t)val;
			}
		}
		catch (...)
		{
			return default_val;
		}
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, uint32_t value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] = to_string((unsigned long long)value);
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
float   ParameterFile::get(string name, float default_val)
{
    auto ittr = mParameters.find(name);
    if (ittr != mParameters.end())
    {
        try
        {
            float val = stof(ittr->second);
            return val;
        }
        catch (...)
        {
            return default_val;
        }
    }
    return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, float value)
{
	// extra cast required for VisualStudio prior to VS12
	mParameters[name] = to_string((long double)value);
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
double   ParameterFile::get(string name, double default_val)
{
    auto ittr = mParameters.find(name);
    if (ittr != mParameters.end())
    {
        try
        {
            double val = stod(ittr->second);
            return val;
        }
        catch (...)
        {
            return default_val;
        }
    }
    return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param   name    the name of the parameter
 * @param   value   the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, double value)
{
    // extra cast required for VisualStudio prior to VS12
    mParameters[name] = to_string((long double)value);
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
string ParameterFile::get(string name, string default_val)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		return ittr->second;
	}
	return default_val;
}

/*******************************************************************************
 *
 * Put the specified value into the map for the specified parameter
 *
 * @param	name	the name of the parameter
 * @param	value	the value that will be put into the map
 *
 ******************************************************************************/
void ParameterFile::put(string name, string value)
{
	mParameters[name] = value;
}

/*******************************************************************************
 *
 * If the specified parameter is found in the map, remove it.
 *
 * @param	name	the name of the parameter
 *
 ******************************************************************************/
void ParameterFile::remove(string name)
{
	auto ittr = mParameters.find(name);
	if (ittr != mParameters.end())
	{
		mParameters.erase(ittr);
	}
}

} // namespace gsu
