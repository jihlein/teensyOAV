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

#ifndef SERVOS_H
#define SERVOS_H

#define MOTORPWMFREQ 250
#define MOTORPWMMIN  900
#define MOTORPWMMAX 2100
  
#define ONESHOTFREQ  500
#define ONESHOTMIN   124
#define ONESHOTMAX   252
  
#define ANALOGFREQ     50
#define ANALOGMIN     900
#define ANALOGMAX    2100
  
#define DIGITALFREQ   250
#define DIGITALMIN    900
#define DIGITALMAX   2100

extern volatile uint16_t servoOut[MAX_OUTPUTS];
extern volatile uint16_t servoCmd[MAX_OUTPUTS];

#endif