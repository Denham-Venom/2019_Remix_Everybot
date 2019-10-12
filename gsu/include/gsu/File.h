/*******************************************************************************
 *
 * File: File.h
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
#include <stdio.h>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>
#include <iostream>

/*******************************************************************************
 *
 * This class contains static methods that aid in finding certain file
 * properties
 *
 ******************************************************************************/

namespace gsu
{
class File
{
    public:
        static void create(std::string path, std::fstream &file_stream);
        static bool exists(std::string path);
        static void open(std::string path, std::fstream& file_stream);
        static bool remove(std::string path);
};
}
