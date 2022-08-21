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

#define CAL_STABLE_TIME 200  // Calibration stable timeout
#define CAL_TIMEOUT  5000    // Calibration timeout
#define GYRODIV 4            // Divide by 16 for 2000 deg/s
#define SECOND_TIMER 1000    // Unit of timing for seconds

//***********************************************************
// ROLL, PITCH, YAW mapping for alternate orientation modes
//***********************************************************

// This is the order to return data in ROLL, PITCH, YAW order
const int8_t gyroRpyOrder[NUMBEROFORIENTS][NUMBEROFAXIS] PROGMEM = 
{
  // Model referenced
  //ROLL, PITCH,   YAW
  { ROLL, PITCH,   YAW},  // Up/Back (Normal)
  {PITCH,  ROLL,   YAW},  // Up/Left
  { ROLL, PITCH,   YAW},  // Up/Front (Aft)
  {PITCH,  ROLL,   YAW},  // Up/Right (Sideways)
  
  {  YAW, PITCH,  ROLL},  // Back/Down (PitchUp)
  {  YAW,  ROLL, PITCH},  // Back/Left
  {  YAW, PITCH,  ROLL},  // Back/Up
  {  YAW, ROLL,  PITCH},  // Back/Right
  
  { ROLL, PITCH,   YAW},  // Down/Back (Upside down)
  {PITCH,  ROLL,   YAW},  // Down/Right
  { ROLL, PITCH,   YAW},  // Down/Front
  {PITCH,  ROLL,   YAW},  // Down/Left
  
  {  YAW, PITCH,  ROLL},  // Front/Down
  {  YAW,  ROLL, PITCH},  // Front/Right
  {  YAW, PITCH,  ROLL},  // Front/Up
  {  YAW,  ROLL, PITCH},  // Front/Left
  
  {PITCH,   YAW,  ROLL},  // Left/Down
  { ROLL,   YAW, PITCH},  // Left/Front
  {PITCH,   YAW,  ROLL},  // Left/Up
  { ROLL,   YAW, PITCH},  // Left/Back
  
  {PITCH,   YAW,  ROLL},  // Right/Down (Vertical)
  { ROLL,   YAW, PITCH},  // Right/Back
  {PITCH,   YAW,  ROLL},  // Right/Up
  { ROLL,   YAW, PITCH},  // Right/Front
};

// These are the polarities to return them to the default
const int8_t gyroPol[NUMBEROFORIENTS][NUMBEROFAXIS] PROGMEM = 
{
  // Model referenced
  // ROLL, PITCH, YAW
  { 1, 1, 1},  // Up/Back (Normal)
  {-1, 1 ,1},  // Up/Left
  {-1,-1, 1},  // Up/Front (Aft)
  { 1,-1, 1},  // Up/Right (Sideways)
  
  {-1, 1, 1},  // Back/Down (PitchUp)
  {-1, 1,-1},  // Back/Left
  {-1,-1,-1},  // Back/Up
  {-1,-1, 1},  // Back/Right
  
  { 1,-1,-1},  // Down/Back (Upside down)
  {-1,-1,-1},  // Down/Right
  {-1, 1,-1},  // Down/Front
  { 1, 1,-1},  // Down/Left
  
  { 1,-1, 1},  // Front/Down
  { 1,-1,-1},  // Front/Right
  { 1, 1,-1},  // Front/Up
  { 1, 1, 1},  // Front/Left
  
  {-1,-1, 1},  // Left/Down
  {-1,-1,-1},  // Left/Front
  { 1,-1,-1},  // Left/Up
  { 1,-1, 1},  // Left/Back
  
  { 1, 1, 1},  // Right/Down (Vertical)
  { 1, 1,-1},  // Right/Back
  {-1, 1,-1},  // Right/Up
  {-1, 1, 1},  // Right/Front
};

//************************************************************
// Code
//************************************************************

int16_t gx, gy, gz;
int16_t gyroAdcRaw[NUMBEROFAXIS]; 
int16_t gyroAdc[NUMBEROFAXIS];     // Holds Gyro data - always in RPY order (Combined - for rest of system)
int16_t gyroAdcAlt[NUMBEROFAXIS];  // Holds Gyro data - always in RPY order (Combined - for IMU)
int16_t gyroAdcP1[NUMBEROFAXIS];   // Holds temp Gyro data - always in RPY order (P1)
int16_t gyroAdcP2[NUMBEROFAXIS];   // Holds temp Gyro data - always in RPY order (P2)

//***************************************************************
// Fill gyroADC[] and gyroADCalt[] with RPY data appropriate to
// the board orientation and users' P1 referencing setting
//
// Remove offsets and change polarity
// Ensure the the correct zero is removed when working across board orientations.
// {ROLL, PITCH, YAW}, // Up/Back (Normal)
// {YAW, PITCH, ROLL}, // Back/Down (PitchUp) (Roll comes from Yaw gyro and needs Yaw's zero, Yaw comes from Roll gyro and needs Roll's zero)
//
// Note that when MODEL or normal VTOL, gyroADC_P1 = gyroADC_P2.
// Only when EARTH-oriented TS is gyroADC_P1 unique.
//
//***************************************************************

void readGyros(void)  // Conventional orientation
{
  uint8_t i;
  int16_t temp1, temp2, temp3;

  getRawGyros();  // Updates gyroADC_P1[] and gyroADC_P2[]

  for (i = 0; i < NUMBEROFAXIS; i++)  
  {
    // Only need to do this if the orientations differ
    if (config.p1Reference != NO_ORIENT)
    {
      // P1 alternate (original) orientation. Swap zeros so that they match.
      temp1 = (gyroAdcP1[i] - config.gyroZeroP1[i]) * (int8_t)pgm_read_byte(&gyroPol[config.orientationP1][i]);

      // P2 orientation
      temp2 = (gyroAdcP2[i] - config.gyroZeroP2[i]) * (int8_t)pgm_read_byte(&gyroPol[config.orientationP2][i]);

      // Merge the two gyros per transition percentage
      temp3 = scale32(temp1, (100 - transition)) + scale32(temp2, transition);  // Sum the two values

      // Gyro alt is always per orientation
      gyroAdcAlt[i] = temp3;

      // If the P1 reference is MODEL, always use the same gyros as P2
      if (config.p1Reference == MODEL)
      {
        // Use P2 orientation
        gyroAdc[i] = temp2; 
      }
      
      // Otherwise use the merged orientation (EARTH reference).
      else
      {
        // Use merged orientation
        gyroAdc[i] = temp3; 
      }
    }
    // Single-orientation models
    else
    {
      // Change polarity using P2 orientation by default
      gyroAdc[i] = (gyroAdcP2[i] - config.gyroZeroP2[i]) * (int8_t)pgm_read_byte(&gyroPol[config.orientationP2][i]);  
        
      // Copy to alternate set of gyro values
      gyroAdcAlt[i] = gyroAdc[i];
    }
  }
}

//***************************************************************
// Fill gyroADC_P1[] and gyroADC_P2[] with RPY data appropriate to 
// the board orientation.
// Nice as it would be, we cannot remove zeros here as this is the
// routine used by the zeroing calibration routine. Chicken | Egg.
// We also cannot merge P1 and P2 here as each have their own zeros.
//***************************************************************

void getRawGyros(void)
{
  uint8_t i;
  
  // Reassemble data into gyroADC array and down-sample to reduce resolution and noise
  gyroAdcRaw[PITCH] = gx;
  gyroAdcRaw[ROLL]  = gy;
  gyroAdcRaw[YAW]   = gz;

  // Reorient the data as per the board orientation 
  for (i = 0; i < NUMBEROFAXIS; i++)
  {
    // Rearrange the sensors for both orientations
    gyroAdcP1[i] = gyroAdcRaw[(int8_t)pgm_read_byte(&gyroRpyOrder[config.orientationP1][i])] >> GYRODIV;
    gyroAdcP2[i] = gyroAdcRaw[(int8_t)pgm_read_byte(&gyroRpyOrder[config.orientationP2][i])] >> GYRODIV;
  }
}

//***************************************************************
// Calibration routines
//***************************************************************

void calibrateGyrosFast(void)
{
  uint8_t i;
  
  // Work out which orientation we are calibrating.
  // Only need to do this if the orientations differ.
  // Just do P2 if orientations the same.
  // Will not save new calibration when different and not firmly in P1 or p2.
  if ((transition > 95) || (config.p1Reference == NO_ORIENT))
  {
    // Clear gyro zeros for the orientation that we are calibrating
    memset(&config.gyroZeroP2[ROLL],0,(sizeof(int16_t) * NUMBEROFAXIS));

    // Calculate average over 32 reads
    for (i = 0; i < 32; i++)
    {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      getRawGyros();  // Updates gyroADC_P1/P2[] with the correct orientation-based RPY

      config.gyroZeroP2[ROLL]  += gyroAdcP2[ROLL];
      config.gyroZeroP2[PITCH] += gyroAdcP2[PITCH];
      config.gyroZeroP2[YAW]   += gyroAdcP2[YAW];
    }

    // Average readings for all axis
    for (i = 0; i < NUMBEROFAXIS; i++)
    {
      config.gyroZeroP2[i] = (config.gyroZeroP2[i] >> 5);
    }
  }
  // P1
  else if (transition <= 5)
  {
    // Clear gyro zeros for the orientation that we are calibrating
    memset(&config.gyroZeroP1[ROLL],0,(sizeof(int16_t) * NUMBEROFAXIS));

    // Calculate average over 32 reads
    for (i = 0; i < 32; i++)
    {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      getRawGyros();  // Updates gyroADC_P1/P2[] with the correct orientation-based RPY

      config.gyroZeroP1[ROLL]  += gyroAdcP1[ROLL];
      config.gyroZeroP1[PITCH] += gyroAdcP1[PITCH];
      config.gyroZeroP1[YAW]   += gyroAdcP1[YAW];
    }

    // Average readings for all axis
    for (i = 0; i < NUMBEROFAXIS; i++)
    {
      config.gyroZeroP1[i] = (config.gyroZeroP1[i] >> 5); // Divide by 32
    }
  }
  
  saveConfigToEEPROM();
}

bool calibrateGyrosSlow(void)
{
  float     gyroSmooth[NUMBEROFAXIS];
  int16_t   gyroOld[NUMBEROFAXIS] = {0,0,0};
  uint16_t  stableCounter = 0; 
  uint8_t   axis;
  uint16_t  gyroSeconds = 0;
  bool      gyrosStable = false;

  // Populate Ccnfig.gyroZero[] with ballpark figures
  // This makes slow calibrate setting much more quickly
  calibrateGyrosFast(); 
  
  // Optimise starting point for each board
  for (axis = 0; axis < NUMBEROFAXIS; axis++)
  {
    // Work out which orientation we are calibrating
    // Only need to do this if the orientations differ
    if ((transition > 95) || (config.p1Reference == NO_ORIENT))
    {
      gyroSmooth[axis] = config.gyroZeroP2[axis];
    }
    else
    {
      gyroSmooth[axis] = config.gyroZeroP1[axis];  
    }   
  }
  
  // Wait until gyros stable. Timeout after CAL_TIMEOUT seconds
  while (!gyrosStable && ((gyroSeconds <= CAL_TIMEOUT)))
  {
    // Count elapsed seconds
    if (gyroTimeout > SECOND_TIMER)
    {
      gyroSeconds++;
      gyroTimeout = 0;
    }

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    getRawGyros();

    // Calculate very long rolling average
    for (axis = 0; axis < NUMBEROFAXIS; axis++) 
    {
      gyroSmooth[axis] = ((gyroSmooth[axis] * (float)999) + (float)(gyroAdc[axis])) / (float)1000;
      
      // See if changing
      if (gyroOld[axis] != (int16_t)gyroSmooth[axis])
      {
        gyrosStable = false;
        stableCounter = 0;
      }
    
      // Save old reading
      gyroOld[axis] = (int16_t)gyroSmooth[axis];
    }
    
    // Increment stable counter to measure how long we are still
    stableCounter++;
    
    // If stable for 5 seconds, do a quick calibrate
    if (stableCounter > CAL_STABLE_TIME)
    {
      gyrosStable = true;  
      calibrateGyrosFast();   
    }
    
    delay(1);

    // Otherwise the original saved values are used
  }
  
  // Return success or failure
  return(gyrosStable);
}
