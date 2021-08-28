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

// ************************************************************
// * Code
// ************************************************************

void displayBalance(void)
{
  int16_t xPos, yPos;
  int16_t count = 0;

  while(digitalRead(BUTTON1) != 0)
  {
    updateTransition();  // Update the transition variable

    // Read accels
    // Get the i2c data from the MPU6050
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    readAccels();

    // Refresh accSmooth values
    // Fake the IMU period as accSmooth doesn't need that
    imuUpdate(0);

    count++;
    
    // Only display once per 10 loops
    if (count > 10)
    {
      // Convert acc signal to a pixel position
      xPos = -accelSmooth[PITCH] + 32;
      yPos = -accelSmooth[ROLL]  + 64;

      if (xPos < 0)   xPos = 0;
      if (xPos > 64)  xPos = 64;
      if (yPos < 0)   yPos = 0;
      if (yPos > 128) yPos = 128;

      u8g2.clearBuffer();
      
      // Print bottom markers
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 54, "\x6E");  // Left
      u8g2.setFont(u8g2_font_helvR08_tr);  
      
      // Draw balance meter
      u8g2.drawFrame( 0,  0, 128, 64);  // Border
      u8g2.drawFrame(54, 22,  21, 21);  // Target
      u8g2.drawLine( 64,  8,  64, 56);  // Crosshairs
      u8g2.drawLine( 32, 32,  96, 32);
      u8g2.drawDisc(yPos, xPos, 8);     // Bubble

      u8g2.sendBuffer();
      
      count = 0;
    }
  }
}
