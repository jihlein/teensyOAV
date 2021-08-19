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

void displayRCInput(void)
{
  // Just normal RX display
  while(digitalRead(BUTTON1) != 0)
  {
    if (digitalRead(BUTTON4) == 0)
    {
      centerSticks();
    }

    if (frame_100Hz) {
      frame_100Hz = false;
      updateTransition();
    }
    
    if (frame_500Hz) {
      frame_500Hz = false;
      tasks500Hz();  
    }
  
    if (frame_10Hz)
    {
      frame_10Hz = false;
    
      u8g2.clearBuffer();
    
      lcdDisplayText(480, 0,  0);  // Throttle
      lcdDisplayText(481, 0, 10);
      lcdDisplayText(482, 0, 20);
      lcdDisplayText(483, 0, 30);

      lcdDisplayText(484, 70,  0);  // Gear
      lcdDisplayText(485, 70, 10);
      lcdDisplayText(486, 70, 20);
      lcdDisplayText(487, 70, 30);

      itoa(monopolarThrottle,pBuffer,10);
      u8g2.setCursor(37,  0); u8g2.print(pBuffer);
      itoa(rcInputs[AILERON],pBuffer,10);
      u8g2.setCursor(37, 10); u8g2.print(pBuffer);
      itoa(rcInputs[ELEVATOR],pBuffer,10);
      u8g2.setCursor(37, 20); u8g2.print(pBuffer);
      itoa(rcInputs[RUDDER],pBuffer,10);
      u8g2.setCursor(37, 30); u8g2.print(pBuffer);
      itoa(rcInputs[GEAR],pBuffer,10);
      u8g2.setCursor(100,  0); u8g2.print(pBuffer);
      itoa(rcInputs[AUX1],pBuffer,10);
      u8g2.setCursor(100, 10); u8g2.print(pBuffer);
      itoa(rcInputs[AUX2],pBuffer,10);
      u8g2.setCursor(100, 20); u8g2.print(pBuffer);
      itoa(rcInputs[AUX3],pBuffer,10);
      u8g2.setCursor(100, 30); u8g2.print(pBuffer);

      // Print bottom text and markers
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 55, "\x6E");  // Left
      u8g2.setFont(u8g2_font_helvR08_tr);  
      lcdDisplayText(60, 110, 55);    // Cal.

      // Update buffer
      u8g2.sendBuffer();
    }
  }
}
