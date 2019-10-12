/*******************************************************************************
*
* File: Config.h
*
* Written by:
* 	The Robonauts
* 	FRC Team 118
* 	NASA, Johnson Space Center
* 	Clear Creek Independent School District
*
******************************************************************************/
#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "gsu/ParameterFile.h"

namespace gsu
{

/*******************************************************************************
 *
 * The Config class provides a static wrapper arround an instance of
 * a ParameterFile object, all ParameterFile methods are available as a
 * static interface
 *
 ******************************************************************************/
class Config
{
	public:
		static void init(std::string filename);

		static void loadFile(std::string filename = "");
		static void saveFile(std::string filename = "");

		static void		clear(void);
		static bool		contains(std::string name);

		static bool			get(std::string name, bool default_val = false);
		static int8_t		get(std::string name, int8_t default_val = 0);
		static uint8_t		get(std::string name, uint8_t default_val = 0);
		static int16_t		get(std::string name, int16_t default_val = 0);
		static uint16_t		get(std::string name, uint16_t default_val = 0);
		static int32_t		get(std::string name, int32_t default_val = 0);
		static uint32_t		get(std::string name, uint32_t default_val = 0);
		static float		get(std::string name, float default_val = 0.0);
		static std::string  get(std::string name, std::string default_val = "");

		static void		put(std::string name, bool value);
		static void		put(std::string name, int8_t value);
		static void		put(std::string name, uint8_t value);
		static void		put(std::string name, int16_t value);
		static void		put(std::string name, uint16_t value);
		static void		put(std::string name, int32_t value);
		static void		put(std::string name, uint32_t value);
		static void		put(std::string name, float value);
		static void		put(std::string name, std::string value);

		static void		remove(std::string name);

    private:
        static ParameterFile m_file;
};

}
