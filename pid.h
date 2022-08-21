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

/*********************************************************************
 * pid.h
 ********************************************************************/

#ifndef PID_H
#define PID_H

//***********************************************************
//* Externals
//***********************************************************

extern int16_t pidAccels[FLIGHT_MODES][NUMBEROFAXIS];
extern int16_t pidGyros[FLIGHT_MODES][NUMBEROFAXIS];
extern float   integralAccelVertF[FLIGHT_MODES];
extern int32_t integralGyro[FLIGHT_MODES][NUMBEROFAXIS];

extern float   gyroAvgNoise;

#endif