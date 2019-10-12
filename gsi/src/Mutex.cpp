/*******************************************************************************
 *
 * File: Mutex.cpp
 * 	Generic System Interface class for mutexes
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "gsi/Mutex.h"
#include <stdio.h>

namespace gsi
{
	// When changing to the C++11 implementation, all implementation of these 
	// classes was moved to the header file
	uint8_t g_mutex_unused; // prevents link warning

} // namespace gsi
