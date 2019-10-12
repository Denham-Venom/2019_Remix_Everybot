/*******************************************************************************
 *
 * File: Crc16.h
 *
 * Written by:
 * 	NASA, Johnson Space Center
 *
 ******************************************************************************/
#pragma once

#include <stdint.h>
#include <functional>

namespace gsu
{

/*******************************************************************************
 *
 * The Crc16 class generates a lookup table based on the type of CRC being used,
 * then uses that table to generate a 16 bit CRC. 
 *
 * By default the lookup table will be for a COMMON polynomial and preset; 
 * however, the class can be created for other polynomials and presets.
 *
 ******************************************************************************/
class Crc16
{
	public:
		enum CrcScheme 
		{
			CRC_COMMON, CRC_MODBUS, CRC_IBM, CRC_KERMIT, CRC_DNP, 
			CRC_CCITT, CRC_CCITT_1D0F, CRC_XMODEM
		};

		enum BitDirection
		{
			BIT_NORMAL, BIT_REVERSE
		};

		Crc16(CrcScheme scheme=CRC_COMMON);
		~Crc16(void);

		uint16_t calculate(uint8_t* buffer, uint16_t size);

		void setScheme(CrcScheme scheme);
		void setScheme(uint16_t poly=0xA001, uint16_t preset=0x0000, BitDirection direction=BIT_REVERSE, bool swap_out=false, bool invert_out=false);

	private:
		void initializeTableNormal(uint16_t polynomial);
		void initializeTableReverse(uint16_t polynomial);

		uint16_t(*m_calculate)(uint8_t* buffer, uint16_t size, uint16_t init_value, uint16_t *table);

		uint16_t m_table[256];
		uint16_t m_preset;
};

}
