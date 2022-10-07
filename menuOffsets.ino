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

#include "menuExt.h"

//************************************************************
// Defines
//************************************************************

#define OFFSETSSTART  230  // Start of Menu text items for curves
#define OFFSETSOFFSET 128  // LCD offsets

//************************************************************
// RC menu items
//************************************************************

const uint16_t offsetsMenuText[MAX_OUTPUTS] PROGMEM = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint16_t offsetsMenuOffsets[MAX_OUTPUTS] PROGMEM = 
{OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET,
 OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET};
 
const menuRange_t offsetsMenuRanges[MAX_OUTPUTS][NUMBEROFPOINTS+1] PROGMEM = 
{
  {
    // OUT1 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT2 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT3 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT4 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },  
  {
    // OUT5 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT6 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT7 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT8 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT9 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  },
  {
    // OUT10 offset curve (8)
    {-125,125,1,0,0},          // Point 1
    {-125,125,1,0,0},          // Point 2
    {-125,125,1,0,0},          // Point 3
    {-125,125,1,0,0},          // Point 4
    {-125,125,1,0,0},          // Point 5
    {-125,125,1,0,0},          // Point 6
    {-125,125,1,0,0},          // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel (unused)
  }  
}; 

// ************************************************************
// * Main menu-specific setup
// ************************************************************

void menuOffsets(void)
{
  int8_t   *valuePtr;
  uint16_t reference = OFFSETSSTART;

  // If sub-menu item has changed, reset sub-menu positions
  if (menuFlag)
  {
    subTop = OFFSETSSTART;     
    menuFlag = 0;
  }

  while (button != BACK)
  {
    valuePtr = &config.offsets[0].point1;

    // Print top level menu
    printMenuItems(subTop, reference, valuePtr, (const unsigned char*)offsetsMenuRanges, 0, (const uint16_t*)offsetsMenuOffsets, (const uint16_t*)offsetsMenuText, cursor);

    // Handle menu navigation
    updateMenu(MAX_OUTPUTS, reference, 0, button, &cursor, &subTop, &menuTemp);

    // Edit selected curve
    if (button == ENTER)
    {
      editCurveItem(menuTemp - reference, OFFSET);  // Curves after NUMBEROFCURVES are offsets
    }

    // Stop unwanted exit to main menu
    if (button == ABORT)
    {
      waitButton1();  // Wait for user's finger off the button
      button = NONE;
    }

    // Save and exit
    if (button == ENTER)
    {
      saveConfigToEEPROM();  // Save value and return
      waitButton4();         // Wait for user's finger off the button
      waitButton1(); 
    }
  }
}
