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

void displaySticks(void)
{
  int8_t  i;
  int8_t  offset;
  int8_t  tempAileron, tempElevator, tempRudder;
  bool    calibrateDone    = false;
  bool    calibrateStarted = false;

  // Save original settings in case user aborts
  tempAileron  = config.aileronPol;
  tempElevator = config.elevatorPol;
  tempRudder   = config.rudderPol;

  // Reset to defaults - not ideal, but it works
  config.aileronPol  = NORMAL;
  config.elevatorPol = NORMAL;
  config.rudderPol   = NORMAL;

  // Until exit button pressed or complete
  while((digitalRead(BUTTON1) != 0) && (!calibrateDone))
  {
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
      
      // Draw first stick on the left    
      offset = 0;

      u8g2.clearBuffer();

      // Draw graphic
      for (i = 0; i < 2; i++)
      {
        u8g2.drawFrame(17 + offset, 0, 40, 40);           // Box
        u8g2.drawLine( 38 + offset, 20, 48 + offset, 3);  // Line 1
        u8g2.drawLine( 41 + offset, 21, 56 + offset, 6);  // Line 2
        u8g2.drawDisc( 38 + offset, 21,  2);              // Center
        u8g2.drawDisc( 51 + offset,  5,  4);              // End

        // Draw second stick on the right
        offset = 52;
      }

      // Print bottom text and markers
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 54, "\x6E");  // Left
      u8g2.setFont(u8g2_font_helvR08_tr);  
      
      // If uncalibrated
      if (!calibrateDone)
      {
        // Display warning if sticks not centered or no RC signal while not started calibrating
        if ((sbusRx.lost_frame() || sbusRx.failsafe()) && !calibrateStarted)
        {
          lcdDisplayText(135, 16 ,43);  // "No RX signal?"
        }
        // Sticks have not moved far enough but RC being received
        else if (((rcInputs[AILERON]  < 500) && (rcInputs[AILERON]  > -500)) ||
                 ((rcInputs[ELEVATOR] < 500) && (rcInputs[ELEVATOR] > -500)) ||
                 ((rcInputs[RUDDER]   < 500) && (rcInputs[RUDDER]   > -500)))
        {
          calibrateStarted = true;
          lcdDisplayText(136, 9, 43);  // "Hold as shown"
        }
        // Sticks should now be in the right position
        // Reverse wrong input channels
        else
        {
          if (rcInputs[AILERON] < 0)
          {
            config.aileronPol = REVERSED;
          }

          if (rcInputs[ELEVATOR] < 0)
          {
            config.elevatorPol = REVERSED;
          }

          if (rcInputs[RUDDER] < 0)
          {
            config.rudderPol = REVERSED;
          }

          // If all positive - done!
          if ((rcInputs[AILERON] > 0) && (rcInputs[ELEVATOR] > 0) && (rcInputs[RUDDER] > 0))
          {
            calibrateDone = true;
          }
        }
      }

      u8g2.sendBuffer();
    }
  
    // Save value and return
    if (calibrateDone)
    {
      lcdDisplayText(137, 52, 43);  // "Done!"
      // Update buffer
      u8g2.sendBuffer();
      // Pause so that the "Done!" text is readable
      saveConfigToEEPROM();
      delay(2000);
    }
    else
    {
      // Restore old settings if failed
      config.aileronPol  = tempAileron;
      config.elevatorPol = tempElevator;
      config.rudderPol   = tempRudder;
    }
  }
}
