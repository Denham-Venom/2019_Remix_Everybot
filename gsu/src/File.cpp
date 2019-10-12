/*******************************************************************************
 *
 * File: File.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/

#include "gsu/File.h"

using namespace gsu;

bool File::exists(std::string path)
{
    struct stat buffer;

    return (stat(path.c_str(), &buffer) == 0);
}

void File::create(std::string path, std::fstream& file_stream)
{
    std::ofstream outfile;
    outfile.open(path.c_str());
    outfile.close();

    File::open(path, file_stream);
}

void File::open(std::string path, std::fstream& file_stream)
{
    file_stream.open(path.c_str(), std::ios_base::out | std::ios_base::in);

    if (file_stream.is_open())
    {
        return;
    }
    else
    {
        file_stream.close();

        throw std::invalid_argument("Could not find a file from the given path.");
    }
}

bool File::remove(std::string path)
{
    return (std::remove(path.c_str()) == 0);
}
