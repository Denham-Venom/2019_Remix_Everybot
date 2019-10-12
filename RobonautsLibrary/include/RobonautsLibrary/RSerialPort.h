/*----------------------------------------------------------------------------*/
/* Robonauts abstraction of IR analog sensor                                  */
/* Initially used during 2018 for line sensors                                */
/*----------------------------------------------------------------------------*/

#pragma once

#ifdef _2018
#include "SerialPort.h"
#else
#include "frc/SerialPort.h"
#endif
using namespace frc;

/**
 * Wrapper class for the WPI SerialPort class
 */
class RSerialPort : SerialPort
{
  public:
    RSerialPort(int port, int msg_size, int baud_rate);
    ~RSerialPort();

    void reset();
    void flush();
    int readData();
    int writeData();

    void setMsgSize(int msg_size);
    int getMsgSize();

    void setBuffer(char* buffer);
    char* getBuffer();

  private:
    int m_baud_rate;
    int m_port;
    int m_msg_size;
    char *m_buffer;
};
