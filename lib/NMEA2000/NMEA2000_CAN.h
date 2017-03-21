/* 
NMEA2000_CAN.h

Copyright (c) 2015-2017 Timo Lappalainen, Kave Oy, www.kave.fi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Just include this <NMEA2000_CAN.h> to your project and it will
  automatically select suitable CAN library according to board
  selected on Arduino development environment. You can still force
  library by adding one of next defines before including library:
  #define USE_N2K_CAN 1  // for use with SPI and MCP2515 can bus controller
  #define USE_N2K_CAN 2  // for use with due based CAN
  #define USE_N2K_CAN 3  // for use with Teensy 3.1/3.2 boards
  #define USE_N2K_CAN 4  // for use with avr boards
  
  There are also library specific defines:
  mcp_can:
    #define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
    #define N2k_CAN_INT_PIN 21 // Use interrupt  and it is connected to pin 21
    #define USE_MCP_CAN_CLOCK_SET 8  // possible values 8 for 8Mhz and 16 for 16 Mhz clock
  */

#ifndef _NMEA2000_CAN_H_
#define _NMEA2000_CAN_H_

#include "N2kMsg.h"
#include "NMEA2000.h"

#define USE_N2K_MCP_CAN 1
#define USE_N2K_DUE_CAN 2
#define USE_N2K_TEENSY_CAN 3
#define USE_N2K_AVR_CAN 4

// Select right CAN according to prosessor
#if !defined(USE_N2K_CAN)
#if defined(__SAM3X8E__)
#define USE_N2K_CAN USE_N2K_DUE_CAN
#elif defined(__MK20DX256__)||defined(__ATMEGA32U4__) || defined(__MK64FX512__) || defined (__MK66FX1M0__)
#define USE_N2K_CAN USE_N2K_TEENSY_CAN
#elif defined(__AVR_AT90CAN32__)||defined(__AVR_AT90CAN64__)||defined(__AVR_AT90CAN128__)|| \
      defined(__AVR_ATmega32C1__)||defined(__AVR_ATmega64C1__)||defined(__AVR_ATmega16M1__)||defined(__AVR_ATmega32M1__)|| defined(__AVR_ATmega64M1__)
#define USE_N2K_CAN USE_N2K_AVR_CAN
#else
#define USE_N2K_CAN USE_N2K_MCP_CAN
#endif
#endif

#if USE_N2K_CAN == USE_N2K_DUE_CAN
// Use Arduino Due internal CAN with due_can library
#include <due_can.h>         // https://github.com/collin80/due_can
#include <NMEA2000_due.h>
tNMEA2000_due NMEA2000;

#elif USE_N2K_CAN == USE_N2K_TEENSY_CAN
// Use Teensy 3.1&3.2 board internal CAN FlexCAN library
#include <FlexCAN.h>
#include <NMEA2000_teensy.h>    // https://github.com/sarfata/NMEA2000_teensy
tNMEA2000_teensy NMEA2000;

#elif USE_N2K_CAN == USE_N2K_AVR_CAN
// Use Atmel AVR internal CAN controller with avr_can library
#include <avr_can.h>            // https://github.com/thomasonw/avr_can
#include <NMEA2000_avr.h>       // https://github.com/thomasonw/NMEA2000_avr
tNMEA2000_avr NMEA2000;

#else  // Use USE_N2K_MCP_CAN
// Use mcp_can library e.g. with Arduino Mega and external MCP2551 CAN bus chip
// CAN_BUS_shield libraries will be originally found on https://github.com/Seeed-Studio/CAN_BUS_Shield
// There is improved library, which branch can be found on https://github.com/peppeve/CAN_BUS_Shield
// That works also with Maple mini and 8 MHz clock. Hopefully these improvements will be applied to
// original library

#if defined(__STM32F1__) // Maple
#include <MapleIntCompatibility.h>
#endif

#include <SPI.h>
#include <mcp_can.h> // https://github.com/peppeve/CAN_BUS_Shield

#if !defined(N2k_CAN_INT_PIN)
#define N2k_CAN_INT_PIN 0xff   // No interrupt.
#ifndef MCP_CAN_RX_BUFFER_SIZE
#define MCP_CAN_RX_BUFFER_SIZE 1   // Just small buffer to save memory
#endif
#endif

#include <NMEA2000_mcp.h>
#if !defined(N2k_SPI_CS_PIN)
#define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
#endif

#if !defined(USE_MCP_CAN_CLOCK_SET)
#define USE_MCP_CAN_CLOCK_SET 16
#endif

#if USE_MCP_CAN_CLOCK_SET == 8
#define MCP_CAN_CLOCK_SET MCP_8MHz
#else
#define MCP_CAN_CLOCK_SET MCP_16MHz
#endif

tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN,MCP_CAN_CLOCK_SET,N2k_CAN_INT_PIN,MCP_CAN_RX_BUFFER_SIZE);

#endif

#endif