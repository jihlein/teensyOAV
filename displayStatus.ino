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

void displayStatus(void)
{
  int16_t temp;
  
  u8g2.clearBuffer();

  // Display text
  lcdDisplayText(264,  0,  0);  // Version text
  lcdDisplayText(266,  0, 12);  // RX sync
  lcdDisplayText(267,  0, 24);  // Profile
  lcdDisplayText( 23, 86, 24);  // Pos
    
  // Display menu and markers
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  u8g2.drawStr(0, 55, "\x6D");  // Down
  u8g2.setFont(u8g2_font_helvR08_tr);
  lcdDisplayText(14, 10, 55);   // Menu

  // Display values
  printMenuText(0, 1, (396 + config.rxMode), 42, 12);  // rx mode
  u8g2.setCursor(110, 24); 
  u8g2.print(transition);  // Raw transition value
  
  // Display transition point
  if (transition <= 0)
  {
    lcdDisplayText(48, 42, 24);
  }
  else if (transition >= 100)
  {
    lcdDisplayText(50, 42, 24);
  }
  else if (transition == config.transitionP1n)
  {
    lcdDisplayText(49, 42, 24);
  }
  else if (transition < config.transitionP1n)
  {
    lcdDisplayText(51, 42, 24);
  }
  else
  {
    lcdDisplayText(52, 42, 24);
  }
  
  // Don't display battery text if there are error messages
  if (generalError == 0)
  {
    // Display voltage
    uint8_t xLoc = 42;   // X location of voltage display
    uint8_t yLoc = 36;   // Y location of voltage display

    lcdDisplayText(289, 0, 36);  // Battery

    // HJI vBatTemp = GetVbat();

    uint16_t vBatTemp = 1680;
    
    temp = vBatTemp / 100;  // Display whole decimal part first
    itoa(temp,pBuffer,10);  u8g2.drawStr(xLoc, yLoc, pBuffer);

    uint8_t pos1, pos2, pos3;
    
    pos1 = u8g2.getStrWidth(pBuffer);
    
    vBatTemp = vBatTemp - (temp * 100); // Now display the parts to the right of the decimal point

    lcdDisplayText(268, (xLoc + pos1 + 1), yLoc);
    
    pos3 = u8g2.getStrWidth(".");
    pos2 = u8g2.getStrWidth("0");
    
    if (vBatTemp >= 10)
    {
      itoa(vBatTemp, pBuffer, 10);  u8g2.drawStr((xLoc + pos1 + pos3 + 3), yLoc, pBuffer);
    }
    else
    {
      lcdDisplayText(269, (xLoc + pos1 + pos3 + 3), yLoc);
      itoa(vBatTemp, pBuffer, 10);  u8g2.drawStr((xLoc + pos1 + pos2 + pos3 + 4), yLoc, pBuffer);
    }
  
    // Display vibration info is set to "ON"
    if (config.vibration == ON)
    {
      // Create message box
      u8g2.setDrawColor(0);
      u8g2.drawBox(29, 11, 70, 42);   // Black box
      u8g2.setDrawColor(1);
      u8g2.drawFrame(29, 11, 70, 42);  // White Outline

      // Display vibration data
      temp = (int16_t)gyroAvgNoise;
      
      itoa(temp, pBuffer, 10);
      uint8_t strLength = strlen(pBuffer);
      u8g2.setCursor(((128 - strLength * 7) / 2), 26); 
      u8g2.print(pBuffer); 
    }  
  }  
  else // Display error messages
  {
    // Prioritise error from top to bottom
    if (generalError & (1 << LVA_ALARM))
    {
      lcdDisplayText(134, 28, 37);  // BATTERY LOW
    }
    else if (generalError & (1 << NO_SIGNAL))
    {
      lcdDisplayText(75, 35, 37);  // NO SIGNAL
    }
    else if (generalError & (1 << THROTTLE_HIGH))
    {
      lcdDisplayText(45, 24, 37);  // THROTTLE HIGH
    }
    else if (generalError & (1 << DISARMED))
    {
      lcdDisplayText(18, 36, 37);  // DISARMED
    }
  }
  
  u8g2.sendBuffer();
}
