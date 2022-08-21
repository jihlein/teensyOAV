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

#ifndef MENUEXT_H
#define MENUEXT_H

// Externs

extern const menuRange_t offsetsMenuRanges[MAX_OUTPUTS][NUMBEROFPOINTS+1] PROGMEM;

// Menu defines
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

// Global menu variables
extern uint8_t  button;
extern uint16_t cursor;
extern uint8_t  menuFlag;
extern uint16_t menuTemp;
extern uint16_t subTop;

extern const uint8_t lines[4];

#endif