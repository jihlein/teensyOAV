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
// * Defines
// ************************************************************

#define SERVOSTART 230  // Start of OUT1-OUT8 Menu text items
#define SERVOITEMS 10   // Number of menu items
#define SERVOOFFSET 80  // LCD horizontal offsets

//************************************************************
// Servo menu items
//************************************************************
   
const uint16_t servoMenuText[3][SERVOITEMS] PROGMEM = 
{
  {141,141,141,141,141,141,141,141,141,141},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
};

const uint16_t servoMenuOffsets[3][SERVOITEMS] PROGMEM =
{
  {SERVOOFFSET,80,80,80,80,80,80,80,80,80},
  {80,80,80,80,80,80,80,80,80,80},
  {80,80,80,80,80,80,80,80,80,80},
};

// As all of these are the same, a new range cloning menu option is used
// to save a lot of PROGMEM space
const menuRange_t servoMenuRanges[3][1] PROGMEM = 
{
  {
    {OFF, ON,1,1,OFF},  // Reverse
  },
  {
    {-125,0,1,3,-100},  // Min travel
  },
  {
    {0,125,1,3,100},    // Max travel
  },
};

//************************************************************
// Servo menu-specific setup
//************************************************************

void menuServoSetup(uint8_t section)
{
  int8_t *valuePtr = &config.servoReverse[0];

  menuRange_t range;
  uint8_t textLink;
  uint8_t i = 0;
  bool  servoEnable = false;
  bool  zeroSetting = false;

  // If submenu item has changed, reset submenu positions
  if (menuFlag)
  {
    subTop = SERVOSTART;
    menuFlag = 0;
  }

  // Get menu offsets
  // 1 = Reverse, 2 = Min, 3 = Max
  while(button != BACK)
  {
    // Load values from eeprom
    for (i = 0; i < SERVOITEMS; i++)
    {
      switch(section)
      {
        case 1:
          break;
          
        case 2:
          valuePtr = &config.minTravel[0];
          servoEnable = true;
          zeroSetting = true;
          break;
          
        case 3:
          valuePtr = &config.maxTravel[0];
          servoEnable = true;
          zeroSetting = true;
          break;
          
        default:
          break;
      }
    }

    // Print menu
    printMenuItems(subTop, SERVOSTART, valuePtr, (const unsigned char*)servoMenuRanges[section - 1], 1, (const uint16_t*)servoMenuOffsets[section - 1], (const uint16_t*)servoMenuText[section - 1], cursor);

    // Handle menu changes
    updateMenu(SERVOITEMS, SERVOSTART, 0, button, &cursor, &subTop, &menuTemp);
    range = getMenuRange ((const unsigned char*)servoMenuRanges[section - 1], 0);

    if (button == ENTER)
    {
      textLink = pgm_read_word(&servoMenuText[section - 1][menuTemp - SERVOSTART]);

      // Zero limits if adjusting
      if (zeroSetting)
      {
        valuePtr[menuTemp - SERVOSTART] = 0;
      }

      // Do not allow servo enable for throttle if in CPPM mode
      if ((config.channel[menuTemp - SERVOSTART].p1SourceA == THROTTLE) && (config.rxMode == CPPM_MODE))
      {
        servoEnable = false;
      }

      doMenuItem(menuTemp, valuePtr + (menuTemp - SERVOSTART), 1, range, 0, textLink, servoEnable, (menuTemp - SERVOSTART));
    }

    // Disable servos
    servoEnable = false;

    if (button == ENTER)
    {
      updateLimits();       // Update actual servo trims
      saveConfigToEEPROM();  // Save value and return
      waitButton4();         // Wait for user's finger off the button
    }
  }
}
