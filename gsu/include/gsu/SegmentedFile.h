/*******************************************************************************
 *
 * File: SegmentedFile.h
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 * 
 ******************************************************************************/
#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>

/*******************************************************************************
 *
 ******************************************************************************/
namespace gsu
{
class SegmentedFile
{
    public:
        SegmentedFile(std::string path, std::string name, std::string ext, uint32_t max_segments);
        virtual ~SegmentedFile(void);

        std::string getPath(void);
        std::string getName(void);
        std::string getExtension(void);

        uint32_t getMaxSegments(void);
        int32_t  getCurrentIndex(void);

        virtual FILE *openNextSegment(std::string aux_info=std::string(""));
        std::string getSegmentName(uint16_t index, std::string aux_info=std::string(""));

    protected:
        uint16_t getLastSegmentIndex(void);
        void	 putNextSegmentIndex(uint16_t index);

        std::string getIndexFileName(void);
    private:
        std::string m_path;
        std::string m_name;
        std::string m_extension;

        int32_t m_current_index;
        uint32_t m_max_segments;
};
}
