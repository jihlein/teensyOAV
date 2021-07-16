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

void menuLog(void)
{
  int8_t  logStart = 0;
  
  while(digitalRead(BUTTON1) != 0)
  {
    if (digitalRead(BUTTON4) == 0)
    {
      // Erase log
      memset(&config.log[0],0,LOGLENGTH);
      config.logPointer = 0;

      // Save log and log pointer
      saveConfigToEEPROM();      
    }
    
    if (digitalRead(BUTTON2) == 0)
    {
      logStart--;
      
      if (logStart < 0)
      {
        logStart = 0;
      }
    }

    if (digitalRead(BUTTON3) == 0)
    {
      logStart++;
      
      if (logStart >= (LOGLENGTH - 5))
      {
        logStart = 15;
      }
    }

    u8g2.clearBuffer();
    
    // Print each line
    for (uint8_t i = 0; i < 5; i++)
    {
      lcdDisplayText(283 + config.log[logStart + i], 0, (i * 10)); // Throttle
    }

    printMenuFrame(LOG);

    u8g2.sendBuffer();
    
    delay(100);
  }
}
