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

// **************************************************************************
// * Defines
// **************************************************************************

// #define ERROR_LOG

#define ITEMOFFSET 10   // Left edge of menu text
#define CURSOROFFSET 3  // Left edge of cursor
#define PREVLINE 2      // When cursor has been asked to move up off screen
#define LINE0 3         // Top line of menu
#define LINE1 15        // Top line of menu
#define LINE2 27        // Middle line of menu
#define LINE3 39        // Bottom line of menu
#define NEXTLINE 40     // When cursor has been asked to move down off screen
#define BACK  0x70      // S1 pressed
#define UP    0xB0      // S2 pressed
#define DOWN  0xD0      // S3 pressed
#define ENTER 0xE0      // S4 pressed
#define NONE  0xF0      // No button pressed
#define ABORT 0xA0      // Abort button pressed

//************************************************************
// Shared defines
//************************************************************

#define CURVESSTARTE  403  // Start of Menu text items for curves
#define CURVESOFFSET  128  // LCD offsets

#define OFFSETSSTART  230  // Start of Menu text items for curves
#define OFFSETSOFFSET 128  // LCD offsets

//************************************************************
// Shared menu items
//************************************************************

uint8_t menuMode = IDLE;

const uint16_t curvesMenuText[NUMBEROFCURVES] PROGMEM = 
{
  0, 0, 0, 0, 0, 0
};

const uint16_t curvesMenuOffsets[NUMBEROFCURVES] PROGMEM = 
{
  CURVESOFFSET, CURVESOFFSET, CURVESOFFSET, CURVESOFFSET, CURVESOFFSET, CURVESOFFSET
};

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

const uint16_t offsetsMenuText[MAX_OUTPUTS] PROGMEM = 
{
  0, 0, 0, 0, 0, 0, 0, 0
};

const uint16_t offsetsMenuOffsets[MAX_OUTPUTS] PROGMEM = 
{
  OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, 
  OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET, OFFSETSOFFSET
};

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
};
