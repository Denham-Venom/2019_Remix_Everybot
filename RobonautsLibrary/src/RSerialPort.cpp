/*******************************************************************************
 *
 * File: RAbsPosSensor.cpp
 * 
 * This file contains the definition of a class for interacting with an
 * Absolute Position Sensor.
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 ******************************************************************************/
#include "RobonautsLibrary/RSerialPort.h"

using namespace frc;

/******************************************************************************
 * 
 * Create an interface to an APS plugged into the specified channel.
 * 
 * @param	channel	the channel or port that the pot is in
 * 
 ******************************************************************************/
RSerialPort::RSerialPort(int port, int msg_size, int baud_rate)
	: SerialPort(baud_rate, (SerialPort::Port)port)
{
	m_port = port;
	m_msg_size = msg_size;
	m_baud_rate = baud_rate;
	m_buffer = nullptr;
}

/******************************************************************************
 *
 ******************************************************************************/
RSerialPort::~RSerialPort(void)
{
	if (m_buffer != nullptr)
	{
		delete m_buffer;
		m_buffer = nullptr;
	}
}

/******************************************************************************
 *
 ******************************************************************************/
void RSerialPort::reset(void)
{
	Reset();
}

/******************************************************************************
 *
 ******************************************************************************/
void RSerialPort::flush(void)
{
	Flush();
}

/******************************************************************************
 *
 ******************************************************************************/
int RSerialPort::readData(void)
{
	return Read(m_buffer, m_msg_size);
}

/******************************************************************************
 * 
 ******************************************************************************/
int RSerialPort::writeData(void)
{
	return Write(m_buffer, m_msg_size);
}

/******************************************************************************
 * 
 ******************************************************************************/
void RSerialPort::setMsgSize(int msg_size)
{
	m_msg_size = msg_size;
}

/******************************************************************************
 *
 ******************************************************************************/
int RSerialPort::getMsgSize(void)
{
	return m_msg_size;
}

/******************************************************************************
 * 
 ******************************************************************************/
void RSerialPort::setBuffer(char* buffer)
{
	m_buffer = buffer;
}

/******************************************************************************
 * 
 ******************************************************************************/
char *RSerialPort::getBuffer(void)
{
	return m_buffer;
}
