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

#define FLIGHTSTARTE 172  // Start of Menu text items for EARTH
#define FLIGHTSTARTM 328  // Start of Menu text items for MODEL
#define FLIGHTOFFSET  85  // LCD offsets
#define FLIGHTTEXT    38  // Start of value text items
#define FLIGHTITEMS   20  // Number of menu items

//************************************************************
// RC menu items
//************************************************************

const uint16_t flightMenuText[FLIGHTITEMS] PROGMEM = 
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
  
const uint16_t flightMenuOffsets[FLIGHTITEMS] PROGMEM = 
{
  FLIGHTOFFSET, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85
};

const menuRange_t flightMenuRanges[FLIGHTITEMS] PROGMEM = 
{
  // Flight (20)
  {0,127,1,0,40},    // Roll gyro P
  {0,127,1,0,10},    // Roll gyro I
  {0,125,1,0,10},    // Roll gyro I-limit
  {0,7,1,0,2},       // Roll gyro rate
  {0,127,1,0,10},    // Roll Acc gain
  {-127,127,1,0,0},  // Roll Acc trim

  {0,127,1,0,40},    // Pitch gyro P
  {0,127,1,0,10},    // Pitch gyro I
  {0,125,1,0,10},    // Pitch gyro I-limit
  {0,7,1,0,2},       // Pitch gyro rate
  {0,127,1,0,10},    // Pitch Acc gain     
  {-127,127,1,0,0},  // Pitch Acc trim

  {0,127,1,0,60},    // Yaw gyro P
  {0,127,1,0,40},    // Yaw gyro I
  {0,125,1,0,25},    // Yaw gyro I-limit
  {0,7,1,0,2},       // Yaw gyro rate
  {-127,127,1,0,0},  // Yaw trim

  {0,127,1,0,40},    // Z Acc P gain
  {0,127,1,0,20},    // Z Acc I gain
  {0,125,1,0,10},    // Z Acc I-limit
};

//************************************************************
// Main menu-specific setup
//************************************************************

void menuFlight(uint8_t mode)
{
  int8_t   *valuePtr;
  menuRange_t range;
  uint8_t  textLink;
  uint16_t reference;

  // Set the correct text list for the selected reference
  if ((config.p1Reference == MODEL) && (mode == P1))
  {
    reference = FLIGHTSTARTM;
  }
  else
  {
    reference = FLIGHTSTARTE;
  }

  // If sub-menu item has changed, reset sub-menu positions
  if (menuFlag)
  {
    if ((config.p1Reference == MODEL) && (mode == P1))
    {
      subTop = FLIGHTSTARTM;   
    }
    else
    {
      subTop = FLIGHTSTARTE;     
    }

    menuFlag = 0;
  }

  while (button != BACK)
  {
    valuePtr = &config.flightMode[mode].rollPMult;

    // Print menu
    printMenuItems(subTop, reference, valuePtr, (const unsigned char*)flightMenuRanges, 0, (const uint16_t*)flightMenuOffsets, (const uint16_t*)flightMenuText, cursor);

    // Handle menu changes
    updateMenu(FLIGHTITEMS, reference, 0, button, &cursor, &subTop, &menuTemp);
    range = getMenuRange((const unsigned char*)flightMenuRanges, (menuTemp - reference));

    if (button == ENTER)
    {
      textLink = pgm_read_word(&flightMenuText[menuTemp - reference]);
      doMenuItem(menuTemp, valuePtr + (menuTemp - reference), 1, range, 0, textLink, false, 0);
    }

    // Update limits when exiting
    if (button == ENTER)
    {
      updateLimits();      // Update I-term limits and triggers based on percentages
      saveConfigToEEPROM();  // Save value and return
      waitButton4();         // Wait for user's finger off the button
    }
  }
}
