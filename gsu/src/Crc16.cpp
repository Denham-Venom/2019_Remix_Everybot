/*******************************************************************************
 *
 * File: Crc16.cpp
 *
 * Written by:
 * 	NASA, Johnson Space Center
 *
 ******************************************************************************/
#include <gsu/Crc16.h>

#include <stdio.h>

/*******************************************************************************
 *
 * This collection of internal functions is used to calculate the CRC, when the
 * CRC scheme is selected a pointer is set to one of these functions.
 *
 * The name of the function is based on the the selected scheme as follows:
 *     calculate_a_b_c, where 
 *        a -> the bit direction N=normal, R=reverse
 *        b -> swap_out S=swap, N=no swap
 *        c -> invert_out I=invert, N=no invert
 *
 * @param buffer	the data buffer
 * @param size		the number of bytes the data
 * @param crc_value the initial crc value (the preset)
 * @param table		a pointer to the lookup table that should be used
 *
 * @return		the CRC for the buffer
 *
 ******************************************************************************/
uint16_t calculate_N_N_N(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	for (uint16_t i = 0; i < size; i++)
	{
		crc_value = (crc_value << 8) ^ table[((crc_value >> 8) ^ buffer[i]) & 0x00ff];
	}

	return crc_value;
}

/******************************************************************************/
uint16_t calculate_N_N_I(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	return ~calculate_N_N_N(buffer, size, crc_value, table);
}

/******************************************************************************/
uint16_t calculate_N_S_N(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	crc_value = calculate_N_N_N(buffer, size, crc_value, table);

	return ((crc_value & 0xff00) >> 8) | ((crc_value & 0x00ff) << 8);
}

/******************************************************************************/
uint16_t calculate_N_S_I(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	crc_value = ~calculate_N_N_N(buffer, size, crc_value, table);

	return ((crc_value & 0xff00) >> 8) | ((crc_value & 0x00ff) << 8);
}

/******************************************************************************/
uint16_t calculate_R_N_N(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	for (uint16_t i = 0; i < size; i++)
	{
		crc_value = (crc_value >> 8) ^ table[(crc_value ^ buffer[i]) & 0x00ff];
	}

	return crc_value;
}

/******************************************************************************/
uint16_t calculate_R_N_I(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	return ~calculate_R_N_N(buffer, size, crc_value, table);
}

/******************************************************************************/
uint16_t calculate_R_S_N(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	crc_value = calculate_R_N_N(buffer, size, crc_value, table);

	return ((crc_value & 0xff00) >> 8) | ((crc_value & 0x00ff) << 8);
}

/******************************************************************************/
uint16_t calculate_R_S_I(uint8_t* buffer, uint16_t size, uint16_t crc_value, uint16_t *table)
{
	crc_value = ~calculate_R_N_N(buffer, size, crc_value, table);

	return ((crc_value & 0xff00) >> 8) | ((crc_value & 0x00ff) << 8);
}

namespace gsu
{

/*******************************************************************************
 *
 * Create an instance of this CRC class for the specified type of CRC
 *
 * @param scheme 	the type of CRC to use, by default CRC_COMMON
 *
 ******************************************************************************/
Crc16::Crc16(CrcScheme scheme)
{
	setScheme(scheme);
}

/*******************************************************************************
 *
 * Release any resources allocated by this instance.
 *
 ******************************************************************************/
Crc16::~Crc16( void )
{
}

/*******************************************************************************
 *
 ******************************************************************************/
void Crc16::setScheme(CrcScheme scheme)
{
	switch (scheme)
	{
		//                            poly    prefix  bit          swap   invert    
		//                                            direction    out    out
		//                            ------  ------  -----------  -----  ----- 
		default:			setScheme(0xA001, 0x0000, BIT_REVERSE, false, false); break;

		case CRC_COMMON:	setScheme(0xA001, 0x0000, BIT_REVERSE, false, false); break;
		case CRC_MODBUS:	setScheme(0xA001, 0xFFFF, BIT_REVERSE, false, false); break;
		case CRC_IBM:		setScheme(0x8005, 0x0000, BIT_REVERSE, false, false); break;
		case CRC_KERMIT:	setScheme(0x8408, 0x0000, BIT_REVERSE, true,  false); break;
		case CRC_DNP:		setScheme(0xA6BC, 0x0000, BIT_REVERSE, true,  true ); break;
		case CRC_CCITT:		setScheme(0x1021, 0xFFFF, BIT_NORMAL,  false, false); break;
		case CRC_CCITT_1D0F:setScheme(0x1021, 0x1D0F, BIT_NORMAL,  false, false); break;
		case CRC_XMODEM:	setScheme(0x1021, 0x0000, BIT_NORMAL,  false, false); break;

		/*
			There are other CRC16 scheme's, but I have no way of checking
			the results and I don't know the preset values or byte/bit 
			orders. I would need to do more research to implement them.

			case CRC_ARINC:       setScheme(0xA02B, ?,      ?,          ?,    ?    ); break;
			case CRC_CDMA2000:    setScheme(0xC867,	?,      ?,          ?,    ?    ); break;
			case CRC_CHAKRAVARTY: setScheme(0x2F15,	?,      ?,          ?,    ?    ); break;
			case CRC_DECT:        setScheme(0x0589,	?,      ?,          ?,    ?    ); break;
			case CRC_OPENSAFE_A:  setScheme(0x5935,	?,      ?,          ?,    ?    ); break;
			case CRC_OPENSAFE_B:  setScheme(0x755B,	?,      ?,          ?,    ?    ); break;
			case CRC_PROFIBUS:    setScheme(0x1DCF, 0xFFFF, BIT_NORMAL, ?,    true ); break;

			The invert out, could be changed to an xorresult, most cases would be 
			xor 0x0000 (false) or 0xFFFF (true), I did see one refernce to a 
			scheme that used an xor of 0x0001.
		*/
	}
}

/*******************************************************************************
 *
 * This private method is used to initialize the class for the specified 
 * CRC scheme. It initializes a lookup table and selects the correct 
 * calcuate_X_X_X method, a pointer to the calculate method is saved
 * internally to eliminate the need for if checks each time calculate()
 * is called.
 * 
 * @param	polynomial	   the CRC polynomial
 *
 * @param	preset		   the CRC preset (or initial) value
 *
 * @param	direction	   the direction in which bits are evaluated while
 *                         calculating the CRC
 *
 * @param	swap_out		true if the resulting crc should be byte swapped
 *
 * @param	invert_out		true if the resulting crc should be bitwise inverted
 *
 ******************************************************************************/
void Crc16::setScheme(uint16_t polynomial, uint16_t preset, BitDirection direction,
	bool swap_out, bool invert_out)
{
	m_preset = preset;

	if (direction == BIT_NORMAL)
	{
		initializeTableNormal(polynomial);

		if (swap_out)
		{
			if (invert_out)
			{
				m_calculate = calculate_N_S_I;
			}
			else
			{
				m_calculate = calculate_N_S_N;
			}
		}
		else
		{
			if (invert_out)
			{
				m_calculate = calculate_N_N_I;
			}
			else
			{
				m_calculate = calculate_N_N_N;
			}
		}
	}
	else
	{
		initializeTableReverse(polynomial);

		if (swap_out)
		{
			if (invert_out)
			{
				m_calculate = calculate_R_S_I;
			}
			else
			{
				m_calculate = calculate_R_S_N;
			}
		}
		else
		{
			if (invert_out)
			{
				m_calculate = calculate_R_N_I;
			}
			else
			{
				m_calculate = calculate_R_N_N;
			}
		}
	}
}

/*******************************************************************************
 *
 * This public method calculates the CRC for a given buffer, it just calls
 * the appropriate internal method and returns the results.
 *
 * @param	buffer	a pointer to the buffer
 * @param	size	the number of bytes in the buffer
 *
 * @return	the CRC for the buffer
 *
 ******************************************************************************/
uint16_t Crc16::calculate(uint8_t* buffer, uint16_t size)
{
	if (buffer == nullptr)
	{
		return m_preset;
	}

	return m_calculate(buffer, size, m_preset, m_table);
}

/*******************************************************************************
*
* Initialize the CCITT CRC table for a given polynomial.
*
* @param	polynomial   	The polynomial.
*
******************************************************************************/
void Crc16::initializeTableNormal(uint16_t polynomial)
{
	uint16_t element_value;

	for (uint16_t i = 0; i < 256; i++)
	{
		element_value = i << 8;
		for (uint8_t j = 0; j < 8; j++)
		{
			if (element_value & 0x8000)
			{
				element_value = (element_value << 1) ^ polynomial;
			}
			else
			{
				element_value <<= 1;
			}
		}

		m_table[i] = element_value;

		//		if (i % 8 == 0) printf("\n");
		//		printf("%04X, ", element_value);
	}
	//	printf("\n");
}

/*******************************************************************************
*
* Initialize the Common CRC table for a given polynomial.
*
* @param	polynomial   	The polynomial.
*
******************************************************************************/
void Crc16::initializeTableReverse(uint16_t polynomial)
{
	uint16_t element_value;

	for (uint16_t i = 0; i < 256; i++)
	{
		element_value = i;
		for (uint8_t j = 0; j < 8; j++)
		{
			if (element_value & 0x0001)
			{
				element_value = (element_value >> 1) ^ polynomial;
			}
			else
			{
				element_value >>= 1;
			}
		}

		m_table[i] = element_value;

		//		if (i % 8 == 0) printf("\n");
		//		printf("%04X, ", element_value);
	}
	//	printf("\n");
}

} // namespace gsu
