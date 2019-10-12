/*******************************************************************************
 * 
 * File: SegmentedFile.cpp
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsu/SegmentedFile.h"
#include "gsi/Path.h"

using namespace std;
using namespace gsi;
using namespace gsu;

/*******************************************************************************
 *
 ******************************************************************************/
SegmentedFile::SegmentedFile(std::string path, std::string name, std::string ext, uint32_t max_segments)
{
	m_path = path;
	m_name = name;
	m_extension = ext;

	m_current_index = -1;
	m_max_segments = max_segments;
}

/*******************************************************************************
 *
 ******************************************************************************/
SegmentedFile::~SegmentedFile(void)
{
    m_current_index = -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
std::string SegmentedFile::getPath(void)
{
	return m_path;
}

/*******************************************************************************
 *
 ******************************************************************************/
std::string SegmentedFile::getName(void)
{
	return m_name;
}

/*******************************************************************************
 *
 ******************************************************************************/
std::string SegmentedFile::getExtension(void)
{
	return m_extension;
}

/*******************************************************************************
 *
 ******************************************************************************/
int32_t  SegmentedFile::getCurrentIndex(void)
{
    return m_current_index;
}

/*******************************************************************************
 *
 ******************************************************************************/
uint32_t SegmentedFile::getMaxSegments(void)
{
	return m_max_segments;
}

/*******************************************************************************
 *
 * WARNING: The calling function must take ownership of the returned file
 * 			pointer and thus is responsible for closing the file when finished
 * 			using it.
 *
 ******************************************************************************/
FILE *SegmentedFile::openNextSegment(std::string aux_info)
{
    m_current_index = getLastSegmentIndex();

    if (((int)m_current_index - (int)m_max_segments) >= 0)
    {
        string old_name = getSegmentName((uint16_t)(m_current_index-m_max_segments));

        if (Path::fileExists(old_name))
        {
                remove(old_name.c_str());
        }
    }

    FILE *fp = fopen(getSegmentName(m_current_index, aux_info).c_str(), "w");
    if (nullptr == fp)
    {
        m_current_index = -1;
    }
    else
    {
        putNextSegmentIndex(m_current_index+1);
    }

    return (fp);
}

/*******************************************************************************
 *
 ******************************************************************************/
std::string SegmentedFile::getSegmentName(uint16_t index, std::string aux_info)
{
	return m_path + Path::FILE_SEPARATOR + m_name + "_" + to_string(index) + aux_info + "." + m_extension;
}

/*******************************************************************************
 *
 ******************************************************************************/
std::string SegmentedFile::getIndexFileName(void)
{
	return m_path + Path::FILE_SEPARATOR + m_name + "_" + m_extension + ".idx";
}

/*******************************************************************************
 *
 ******************************************************************************/
uint16_t SegmentedFile::getLastSegmentIndex(void)
{
	uint16_t index = 0;

	string fname = getIndexFileName();
	if (Path::fileExists(fname))
	{
		 FILE* file = fopen(fname.c_str(), "r");
		 int i = 0;
		 int cnt = fscanf (file, "%d", &i);
		 if (cnt == 1)
		 {
			 index = (uint16_t)i;
		 }
		 fclose (file);
	}

	return index;
}

/*******************************************************************************
 *
 ******************************************************************************/
void SegmentedFile::putNextSegmentIndex(uint16_t index)
{
	 FILE* file = fopen(getIndexFileName().c_str(), "w");
	 fprintf (file, "%d", (int)index);
	 fclose (file);
}
