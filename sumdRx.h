// **************************************************************************
// * teensyOAV - a port of David Thompsen's OpenAero code to the teensy 4
// *
// * John Ihlein  July 2021
// **************************************************************************

// **************************************************************************
// OpenAero code by David Thompson, included open-source code as per quoted references.
//
// **************************************************************************
// *             GNU GPL V3 notice
// **************************************************************************
// * Copyright (C) 2016 David Thompson
// * 
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// * 
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// * 
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see <http://www.gnu.org/licenses/>.
// * 
// * tl;dr - all derivative code MUST be released with the source code!
// *
// **************************************************************************
//
// Sumd decoding code from:
//
// https://github.com/Benoit3/Sumd-for-Arduino
//
// **************************************************************************

#ifndef SUMDRX
#define SUMDRX

#include <stdint.h>

//SUMD protocol constant
#define MAX_CHANNEL_NUMBER     16  //shall be equal or smaller than 32
#define HEADER_SIZE             3
#define CRC_SIZE                2
#define SUMD_MAX_FRAME_PERIOD 750  //above this delay, switch to failsafe

//SUMD decoding buffer declaration

// buffer size : 1 extra byte for easy empty/full detection using start and stop
#define RING_BUFFER_SIZE (HEADER_SIZE + MAX_CHANNEL_NUMBER * 2 + CRC_SIZE + 1)

class SumdRx {
  private:
    //ring buffer
    // buffer size : 2 Bytes per channel + synchro, failsafe, channel nb, chksum(2) 
    // and 1 extra byte for empty/full detection
    uint8_t ringBuffer[RING_BUFFER_SIZE]; 

    int bufferStart;
    int bufferStop;
    int bufferLength(); // function used to calculate length of data available in the buffer
    
    //decoding status
    bool headerFound;
    int channelNb;      // number of channel available in the frame
    bool failsafe;
    unsigned long lastFrameTime;

    uint16_t crc16_CCITT(uint16_t crc, uint8_t value); // calculate frame crc

  public:
    // HOTT Ctrl typical value
    static const int HOTT_MIN       = 1100;
    static const int HOTT_M50       = 1300;
    static const int HOTT_CENTER    = 1500;
    static const int HOTT_P50       = 1700;
    static const int HOTT_MAX       = 1900;
    static const int HOTT_DEAD_ZONE = 20;
    
    int channelRx;
    int channel[MAX_CHANNEL_NUMBER];
    SumdRx();
    void reset();
    void add(uint8_t rxByte);  // add one by at the end of the buffer
    bool failSafe();           // check the fail safe status
};

#endif