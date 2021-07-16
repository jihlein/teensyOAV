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

//************************************************************
// Misc globals
//************************************************************

// Status LED
#define STATUS_LED 16
#define LED_ON  digitalWrite(STATUS_LED, 1)
#define LED_OFF digitalWrite(STATUS_LED, 0)

uint8_t loopCount = 0;
bool    overdue   = false;
bool    updated   = false;

//************************************************************
// Shared imu globals
//************************************************************

float accelSmooth[NUMBEROFAXIS];  // Filtered accel data
int16_t angle[2];                 // Attitude in degrees - pitch and roll
float intervalF;

//************************************************************
// Shared init globals
//************************************************************

CONFIG_STRUCT config;        // eeProm data configuration
uint16_t systemVoltage = 0;  // Initial voltage measured

//************************************************************
// Shared menu globals
//************************************************************

uint8_t  button;
uint8_t  buttonMultiplier;
uint16_t cursor = LINE0;
uint8_t  menuFlag = 1;
uint16_t menuTemp = 0;
uint16_t subTop   = 0;

//************************************************************
// Shared pid globals
//************************************************************

float   gyroAvgNoise = 0.0;
float   integralAccelVertF[FLIGHT_MODES];          // Integrated Acc Zint32_t integralGyro[FLIGHT_MODES][NUMBEROFAXIS];  // PID I-terms (gyro) for each axis
int32_t integralGyro[FLIGHT_MODES][NUMBEROFAXIS];  // PID I-terms (gyro) for each axis
int16_t pidGyros[FLIGHT_MODES][NUMBEROFAXIS];
int16_t pidAccels[FLIGHT_MODES][NUMBEROFAXIS];

//************************************************************
// Shared rc globals
//************************************************************

int16_t monopolarThrottle;                 // Monopolar throttle
int16_t rcInputs[MAX_RC_CHANNELS + 1];     // Normalised (+/- 1250) RC inputs
int16_t rawChannels[MAX_RC_CHANNELS + 1];  // Raw RC inputs

//************************************************************
// Shared sensor globals
//************************************************************

int16_t ax, ay, az;
int16_t gx, gy, gz;

int16_t accelAdc[NUMBEROFAXIS];    // Holds Acc ADC values - always in RPY order (Combined)
float   accelVertF = 0.0;          // Holds the level-zeroed Z-acc value. Used for height damping in hover only.

int16_t gyroAdcAlt[NUMBEROFAXIS];  // Holds Gyro data - always in RPY order (Combined - for IMU)

//************************************************************
// Shared servo globals
//************************************************************

#define ONESHOTFREQ  500
#define ONESHOTMIN   124
#define ONESHOTMAX   252
  
#define ANALOGFREQ     50
#define ANALOGMIN     900
#define ANALOGMAX    2100
  
#define DIGITALFREQ   250
#define DIGITALMIN    900
#define DIGITALMAX   2100
    
uint16_t servoCmd[MAX_OUTPUTS];
uint16_t servoOut[MAX_OUTPUTS];
