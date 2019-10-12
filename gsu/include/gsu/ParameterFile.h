/********************************************************************************
 *
 * File: ParameterFile.h
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

namespace gsu
{

/*******************************************************************************
 *
 * \brief The ParameterFile class provides an interface for loading a parameter 
 *       file into memory for easy access.
 *
 ******************************************************************************/
class ParameterFile
{
	public:
		ParameterFile(void);
		ParameterFile(std::string filename);

		void loadFile(std::string filename);
		void saveFile(std::string filename = "");

		void		clear(void);
		bool		contains(std::string name);

		bool		get(std::string name, bool default_val = false);
		int8_t		get(std::string name, int8_t default_val = 0);
		uint8_t		get(std::string name, uint8_t default_val = 0);
		int16_t		get(std::string name, int16_t default_val = 0);
		uint16_t	get(std::string name, uint16_t default_val = 0);
		int32_t		get(std::string name, int32_t default_val = 0);
		uint32_t	get(std::string name, uint32_t default_val = 0);
        float       get(std::string name, float default_val = 0.0);
        double      get(std::string name, double default_val = 0.0);
		std::string get(std::string name, std::string default_val = "");

		void		put(std::string name, bool value);
		void		put(std::string name, int8_t value);
		void		put(std::string name, uint8_t value);
		void		put(std::string name, int16_t value);
		void		put(std::string name, uint16_t value);
		void		put(std::string name, int32_t value);
		void		put(std::string name, uint32_t value);
        void        put(std::string name, float value);
        void        put(std::string name, double value);
		void		put(std::string name, std::string value);

		void		remove(std::string name);

	private:
		std::map<std::string, std::string> mParameters;
		std::string mFilename;
};

}
