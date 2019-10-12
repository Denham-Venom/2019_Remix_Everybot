/*******************************************************************************
 *
 * File: Network.h
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#pragma once

#include <stdint.h>

namespace gsi
{
/*******************************************************************************
 *
 * This function pointer is used internally to make conversions without 
 * having to constantly ask about endianess.
 *
 ******************************************************************************/
typedef void (*NetCopyFunction)(uint8_t *dst, uint8_t *src);

/*******************************************************************************
 *
 * This class provides resources for interacting with network interfaces.
 *
 ******************************************************************************/
class Network
{
	public:
		static bool isLittleEndian(void);

        static uint8_t * toNetBytes(uint8_t *b_ptr, int8_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, uint8_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, int16_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, uint16_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, int32_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, uint32_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, int64_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, uint64_t value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, float value);
        static uint8_t * toNetBytes(uint8_t *b_ptr, double value);

        static uint8_t * fromNetBytes(uint8_t *b_ptr, int8_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, uint8_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, int16_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, uint16_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, int32_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, uint32_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, int64_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, uint64_t *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, float *value);
        static uint8_t * fromNetBytes(uint8_t *b_ptr, double *value);

	private:
		static NetCopyFunction netCopy1;
		static NetCopyFunction netCopy2;
		static NetCopyFunction netCopy4;
		static NetCopyFunction netCopy8;
};

} // namespace gsi
