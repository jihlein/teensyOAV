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

#include <DSMRX.h>
#include <MPU6050.h>
#include <myPWMServo.h>
#include <sbus.h>
#include <U8g2lib.h>
#include <Wire.h>

// Uncomment only one of the following OLED dislay drivers:
// I2C Drivers:
// U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// SPI Drivers:
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

MPU6050 mpu;

SbusRx sbusRx(&Serial3);

DSM1024 dsm;

myPWMServo output0;
myPWMServo output1;
myPWMServo output2;
myPWMServo output3;
myPWMServo output4;
myPWMServo output5;
myPWMServo output6;
myPWMServo output7;

// HEARTBEAT_LED
#define HEARTBEAT_LED 3

// LED Heartbeat Variables
uint8_t ledState = LOW;

// Executive Timing Variables
IntervalTimer execTimer;

#define FRAME_COUNT 1000
#define COUNT_500HZ 2
#define COUNT_250HZ 4
#define COUNT_100HZ 10
#define COUNT_50HZ  20
#define COUNT_20HZ  50
#define COUNT_10HZ  100
#define COUNT_5HZ   200
#define COUNT_1HZ   1000

uint16_t frameCounter = 0;
uint8_t frame_500Hz   = false;
uint8_t frame_250Hz   = false;
uint8_t frame_100Hz   = false;
uint8_t frame_50Hz    = false;
uint8_t frame_20Hz    = false;
uint8_t frame_10Hz    = false;
uint8_t frame_5Hz     = false;
uint8_t frame_1Hz     = false;

uint16_t armTimer          = 0;
uint16_t disarmTimer       = 0;
uint16_t gyroTimeout       = 0;
uint16_t rcTimeout         = 0;
uint16_t statusSeconds     = 0;
uint16_t transitionTimeout = 0;
uint16_t updateStatusTimer = 0;

// Button Pins
#define BUTTON1 20
#define BUTTON2 21
#define BUTTON3 22
#define BUTTON4 23

uint8_t pinb = 0xFF;

//***********************************************************
//* Code and Data variables
//***********************************************************

// Flight variables
int16_t transition = 0;
int16_t transitionCounter = 0;

// Flags
volatile uint8_t flightFlags  = 0;
volatile uint8_t generalError = 0;

// Global Buffers
#define PBUFFER_SIZE 25
char pBuffer[PBUFFER_SIZE];      // Print buffer (25 bytes)

// Misc structures
typedef struct
{
  int8_t  lower;         // Lower limit for menu item
  int8_t  upper;         // Upper limit for menu item
  uint8_t increment;     // Increment for menu item
  uint8_t style;         // 0 = numeral, 1 = text
  int8_t  defaultValue;  // Default value for this item
} menuRange_t; 

//************************************************************
//* Setup
//************************************************************

void setup() {
  Serial.begin(115200);
  
  // Call Init Functions
  init();
  
  // Setup Exec ISR
  execTimer.begin(execCount, 1000);  // run every mSec
}

//************************************************************
//* Exec function
//************************************************************

void execCount(void) {
  frameCounter++;
  if (frameCounter > FRAME_COUNT)
    frameCounter = 1;

  if ((frameCounter % COUNT_500HZ) == 0)
    frame_500Hz = true;

  if ((frameCounter % COUNT_250HZ) == 0)
    frame_250Hz = true;
    
  if ((frameCounter % COUNT_100HZ) == 0)
  {
    frame_100Hz = true;
    rxGetChannels();
  }
    
  if ((frameCounter % COUNT_50HZ) == 0)
    frame_50Hz = true;

  if ((frameCounter % COUNT_20HZ) == 0)
    frame_20Hz = true;
    
  if ((frameCounter % COUNT_10HZ) == 0)
    frame_10Hz = true;
    
  if ((frameCounter % COUNT_5HZ) == 0)
    frame_5Hz = true;

  if ((frameCounter % COUNT_1HZ) == 0)
    frame_1Hz = true;

  armTimer++;
  disarmTimer++;
  gyroTimeout++;
  rcTimeout++;
  statusSeconds++;
  transitionTimeout++;
  updateStatusTimer++;
  
  pinb = digitalRead(BUTTON1) << 7 |
         digitalRead(BUTTON2) << 6 |
         digitalRead(BUTTON3) << 5 |
         digitalRead(BUTTON4) << 4;
}

//************************************************************
//* Main loop
//************************************************************

void loop() {
  if (frame_250Hz) {
    menuMethods();
    rxGetChannels();
  }
  
  if (frame_500Hz) {
    frame_500Hz = false;
    tasks500Hz();  
    send500HzServos();
  }
  
  if (frame_250Hz) {
    frame_250Hz = false;
    send250HzServos();
  }
  
  if (frame_100Hz) {
    frame_100Hz = false;
  }
  
  if (frame_50Hz) {
    frame_50Hz = false;
    tasks50Hz();
    send50HzServos();
  }
  
  if (frame_20Hz) {
    frame_20Hz = false;
  }
  
  if (frame_10Hz) {
    frame_10Hz = false;
    //serialDebug10Hz();
  }
  
  if (frame_5Hz) {
    frame_5Hz = false;
  }
  
  if (frame_1Hz) {
    frame_1Hz = false;
    // Blink Heartbeat LED
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
  
    digitalWrite(HEARTBEAT_LED, ledState);
    
    //uint16_t rawBatt = analogRead(17);
  } 
}
