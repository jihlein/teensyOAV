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

//***********************************************************
//* Includes
//***********************************************************

#include "gyros.h"
#include "imu.h"
#include "ioCfg.h"

// ************************************************************
// * Code
// ************************************************************

void displaySensors(void)
{
  bool firstTime = true;
  
  // While BACK not pressed
  while(digitalRead(BUTTON1) != 0)
  {
    previousTime = startTime;      
    startTime = micros();      
    dt = (float)(startTime - previousTime) / 1000000.0f;

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    readGyros();
    readAccels();
 
    imuUpdate(dt);

    u8g2.clearBuffer();
  
    lcdDisplayText(26,  37, 0);  // Gyro
    lcdDisplayText(30,  72, 0);  // Acc
    lcdDisplayText(31, 107, 0);  // IMU

    lcdDisplayText(27, 5, 13);   // Roll
    lcdDisplayText(28, 5, 23);   // Pitch
    lcdDisplayText(29, 5, 33);   // Yaw/Z

    lcdDisplayText(229, 5, 45);  // AccVert
    
    itoa(gyroAdcAlt[ROLL],pBuffer,10);
    u8g2.setCursor(40, 13); u8g2.print(pBuffer);
    itoa(gyroAdcAlt[PITCH],pBuffer,10);
    u8g2.setCursor(40, 23); u8g2.print(pBuffer);
    itoa(gyroAdcAlt[YAW],pBuffer,10);
    u8g2.setCursor(40, 33); u8g2.print(pBuffer);
    itoa(accelAdc[ROLL],pBuffer,10);
    u8g2.setCursor(75, 13); u8g2.print(pBuffer);
    itoa(accelAdc[PITCH],pBuffer,10);
    u8g2.setCursor(75, 23); u8g2.print(pBuffer);
    itoa(accelAdc[YAW],pBuffer,10);
    u8g2.setCursor(75, 33); u8g2.print(pBuffer);
    itoa(angle[ROLL]/100,pBuffer,10);
    u8g2.setCursor(107, 13); u8g2.print(pBuffer);
    itoa(angle[PITCH]/100,pBuffer,10);
    u8g2.setCursor(107, 23); u8g2.print(pBuffer);    
    itoa((int16_t)accelVertF,pBuffer,10);
    u8g2.setCursor(40, 45); u8g2.print(pBuffer);
    
    // Print bottom markers
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawStr(  0, 55, "\x6E");  // Left
    u8g2.setFont(u8g2_font_helvR08_tr);  
    lcdDisplayText(60, 108, 55);  // Calibrate
    lcdDisplayText(25,  75, 55);  // Inverted Calibrate   

    // Update buffer
    u8g2.sendBuffer();
    
    if (firstTime)
    {
      // Wait until finger off button
      waitButton4();
     
      firstTime = false;
    }
    
    // Normal calibrate button pressed
    if (digitalRead(BUTTON4) == 0)
    {
      // Wait until finger off button
      waitButton4();
      
      // Pause until steady
      delay(250);
      
      // Calibrate sensors
      calibrateGyrosFast();
      calibrateAcc(NORMAL);
    }

    // Inverted calibrate button pressed
    if (digitalRead(BUTTON3) == 0)
    {
      // Wait until button snap dissipated
      delay(250);
      calibrateAcc(REVERSED);
    }

    // Delay until 25000 uSec (40 Hz) have elapsed
    elapsedTime = micros();
  
    Serial.print(startTime - previousTime);  Serial.print("\t");
    Serial.println(elapsedTime - startTime);
    
    while (25000 > (elapsedTime - startTime))
      elapsedTime = micros();
  }
}
