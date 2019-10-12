/*******************************************************************************
 *
 * File: Network.cpp
 *	Generic System Interface Time wrapper
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Network.h"

namespace gsi
{

/*******************************************************************************
 *
 * This file scoped method is used to copy 1 byte to the destination, in this
 * case endianess does not matter.
 *
 ******************************************************************************/
void netCopy1AnyEndian(uint8_t * dst, uint8_t *src)
{
	*dst = *src;
}

/*******************************************************************************
 *
 * This file scoped method is used to copy 2 bytes to the destination on a big
 * endian system.
 *
 ******************************************************************************/
inline void netCopy2BigEndian(uint8_t * dst, uint8_t *src)
{
	*dst++ = *src++;
	*dst = *src;
}

/*******************************************************************************
 *
 * This file scoped method is used to copy 2 bytes to the destination on a little
 * endian system.
 *
 ******************************************************************************/
inline void netCopy2LittleEndian(uint8_t * dst, uint8_t *src)
{
	src = src + 1;
	*dst++ = *src--;
	*dst = *src;
}

/*******************************************************************************
 *
 * This file scoped method is used to copy 4 bytes to the destination on a big
 * endian system.
 *
 ******************************************************************************/
inline void netCopy4BigEndian(uint8_t * dst, uint8_t *src)
{
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst = *src;
}

/*******************************************************************************
 *
 * This file scoped method is used to copy 4 bytes to the destination on a little
 * endian system.
 *
 ******************************************************************************/
inline void netCopy4LittleEndian(uint8_t * dst, uint8_t *src)
{
	src = src + 3;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst = *src;
}

/*******************************************************************************
 *
 * This file scoped method is used to copy 8 bytes to the destination on a big
 * endian system.
 *
 ******************************************************************************/
inline void netCopy8BigEndian(uint8_t * dst, uint8_t *src)
{
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst++ = *src++;
	*dst = *src;
}

/*******************************************************************************
 *
 * This file scoped method is used to copy 8 bytes to the destination on a little
 * endian system.
 *
 ******************************************************************************/
inline void netCopy8LittleEndian(uint8_t * dst, uint8_t *src)
{
	src = src + 7;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst++ = *src--;
	*dst = *src;
}

/*******************************************************************************
 *
 * Initialize some function pointers for doing the copies. These are only 
 * initialized once when the program starts so we don't need to ask for 
 * endianess with every conversion -- only a few times at program start.
 *
 ******************************************************************************/
NetCopyFunction Network::netCopy1 = netCopy1AnyEndian;
NetCopyFunction Network::netCopy2 = isLittleEndian() ? netCopy2LittleEndian : netCopy2BigEndian;
NetCopyFunction Network::netCopy4 = isLittleEndian() ? netCopy4LittleEndian : netCopy4BigEndian;
NetCopyFunction Network::netCopy8 = isLittleEndian() ? netCopy8LittleEndian : netCopy8BigEndian;

/*******************************************************************************
 *
 * Used to determine if this is a big or little endian process. I know, there
 * is such a thing as middle endian, but I have never seen a computer system that 
 * uses such a thing so I'm pretending they don't exist -- for now.
 *
 * @return true if this is a little endian process, else return false
 *
 ******************************************************************************/
inline bool Network::isLittleEndian(void)
{
	uint16_t value = 0x1;
	return (*((uint8_t *)&value) == 1);
}
	 
/*******************************************************************************
 *
 * The toNetBytes methods copy the bytes from the value to the location specified by
 * the provided byte pointer. This method is designed to build network ready
 * messages from a collection of values so the return value is the provided
 * byte pointer advanced by the number of bytes that were copied to the 
 * buffer -- that is the next spot in the buffer that should be written to.
 *
 * @param	b_ptr	a pointer into a buffer of bytes that is being used to
 *                  create a network ready message.
 *
 * @param	value	the value that should be put into the buffer
 *
 * @return	a pointer to the next spot in the buffer
 *
 ******************************************************************************/
uint8_t * Network::toNetBytes(uint8_t *b_ptr, int8_t value)      { netCopy1(b_ptr, (uint8_t *)&value); return b_ptr + 1; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, uint8_t value)	 { netCopy1(b_ptr, (uint8_t *)&value); return b_ptr + 1; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, int16_t value)	 { netCopy2(b_ptr, (uint8_t *)&value); return b_ptr + 2; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, uint16_t value)	 { netCopy2(b_ptr, (uint8_t *)&value); return b_ptr + 2; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, int32_t value)	 { netCopy4(b_ptr, (uint8_t *)&value); return b_ptr + 4; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, uint32_t value)	 { netCopy4(b_ptr, (uint8_t *)&value); return b_ptr + 4; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, int64_t value)	 { netCopy8(b_ptr, (uint8_t *)&value); return b_ptr + 8; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, uint64_t value)	 { netCopy8(b_ptr, (uint8_t *)&value); return b_ptr + 8; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, float value)		 { netCopy4(b_ptr, (uint8_t *)&value); return b_ptr + 4; }
uint8_t * Network::toNetBytes(uint8_t *b_ptr, double value)	 	 { netCopy8(b_ptr, (uint8_t *)&value); return b_ptr + 8; }

/*******************************************************************************
 *
 * The fromNetBytes methods copy the bytes from the the location specified by
 * the provided byte pointer to the value pointer. This method is designed 
 * to parse network messages to a collection of values so the return value 
 * is the provided byte pointer advanced by the number of bytes that were 
 * copied to from the buffer -- that is the next spot in the buffer that should
*  be read.
 *
 * @param	b_ptr	a pointer into a buffer of bytes that is being parsed
 *
 * @param	value	a pointer to where the value should be put
 *
 * @return	a pointer to the next spot in the buffer
 *
 ******************************************************************************/
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, int8_t *value)	 { netCopy1((uint8_t *)value, b_ptr); return b_ptr + 1; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, uint8_t *value)	 { netCopy1((uint8_t *)value, b_ptr); return b_ptr + 1; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, int16_t *value)	 { netCopy2((uint8_t *)value, b_ptr); return b_ptr + 2; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, uint16_t *value) { netCopy2((uint8_t *)value, b_ptr); return b_ptr + 2; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, int32_t *value)	 { netCopy4((uint8_t *)value, b_ptr); return b_ptr + 4; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, uint32_t *value) { netCopy4((uint8_t *)value, b_ptr); return b_ptr + 4; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, int64_t *value)	 { netCopy8((uint8_t *)value, b_ptr); return b_ptr + 8; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, uint64_t *value) { netCopy8((uint8_t *)value, b_ptr); return b_ptr + 8; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, float *value)	 { netCopy4((uint8_t *)value, b_ptr); return b_ptr + 4; }
uint8_t * Network::fromNetBytes(uint8_t *b_ptr, double *value)	 { netCopy8((uint8_t *)value, b_ptr); return b_ptr + 8; }
	 																									 
} // namespace gsi
