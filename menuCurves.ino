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

#define CURVESSTARTE 403  // Start of Menu text items for curves
#define CURVESOFFSET 128  // LCD offsets

//************************************************************
// RC menu items
//************************************************************

const uint16_t curvesMenuText[NUMBEROFCURVES] PROGMEM = {0, 0, 0, 0, 0, 0};

const uint16_t curvesMenuOffsets[NUMBEROFCURVES] PROGMEM = 
{CURVESOFFSET, CURVESOFFSET, CURVESOFFSET, CURVESOFFSET, CURVESOFFSET, CURVESOFFSET};

const menuRange_t curvesMenuRanges[NUMBEROFCURVES][NUMBEROFPOINTS+1] PROGMEM = 
{
  {
    // P1 Throttle Curve (8)
    {0,100,1,0,0},             // Point 1
    {0,100,1,0,17},            // Point 2
    {0,100,1,0,33},            // Point 3
    {0,100,1,0,50},            // Point 4
    {0,100,1,0,66},            // Point 5    
    {0,100,1,0,83},            // Point 6
    {0,100,1,0,100},           // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel
  },
  {
    // P2 Throttle Curve (8)
    {0,100,1,0,0},             // Point 1
    {0,100,1,0,17},            // Point 2
    {0,100,1,0,33},            // Point 3
    {0,100,1,0,50},            // Point 4
    {0,100,1,0,66},            // Point 5
    {0,100,1,0,83},            // Point 6
    {0,100,1,0,100},           // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel
  },
  {
    // P1 Collective Curve (8)
    {-100,100,1,0,-100},       // Point 1
    {-100,100,1,0,-66},        // Point 2
    {-100,100,1,0,-33},        // Point 3
    {-100,100,1,0,0},          // Point 4
    {-100,100,1,0,33},         // Point 5
    {-100,100,1,0,66},         // Point 6
    {-100,100,1,0,100},        // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel   
  },
  {
    // P2 Collective Curve (8)
    {-100,100,1,0,-100},       // Point 1
    {-100,100,1,0,-66},        // Point 2
    {-100,100,1,0,-33},        // Point 3
    {-100,100,1,0,0},          // Point 4
    {-100,100,1,0,33},         // Point 5
    {-100,100,1,0,66},         // Point 6
    {-100,100,1,0,100},        // Point 7
    {SRC1, SRC1, 1, 1, SRC1},  // Associated channel
  },
  {
    // Generic Curve C (8)
    {-100,100,1,0,-100},         // Point 1
    {-100,100,1,0,-66},          // Point 2
    {-100,100,1,0,-33},          // Point 3
    {-100,100,1,0,0},            // Point 4
    {-100,100,1,0,33},           // Point 5
    {-100,100,1,0,66},           // Point 6
    {-100,100,1,0,100},          // Point 7
    {SRC5, NOMIX, 1, 1, NOMIX},  // Associated channel
  },
  {
    // Generic Curve D (8)
    {-100,100,1,0,-100},         // Point 1
    {-100,100,1,0,-66},          // Point 2
    {-100,100,1,0,-33},          // Point 3
    {-100,100,1,0,0},            // Point 4
    {-100,100,1,0,33},           // Point 5
    {-100,100,1,0,66},           // Point 6
    {-100,100,1,0,100},          // Point 7
    {SRC5, NOMIX, 1, 1, NOMIX},  // Associated channel
  },
};

// ************************************************************
// * Main menu-specific setup
// ************************************************************

void menuCurves(void)
{
  int8_t   *valuePtr;
  uint16_t reference = CURVESSTARTE;

  // If sub-menu item has changed, reset sub-menu positions
  if (menuFlag)
  {
    subTop = CURVESSTARTE;     
    menuFlag = 0;
  }

  while (button != BACK)
  {
    valuePtr = &config.curve[0].point1;

    // Print top level menu
    printMenuItems(subTop, reference, valuePtr, (const unsigned char*)curvesMenuRanges, 0, (const uint16_t*)curvesMenuOffsets, (const uint16_t*)curvesMenuText, cursor);

    // Handle menu navigation
    updateMenu(NUMBEROFCURVES, reference, 0, button, &cursor, &subTop, &menuTemp);

    // Edit selected curve
    if (button == ENTER)
    {
      editCurveItem(menuTemp - reference, CURVE);
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
