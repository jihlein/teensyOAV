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
//* IMU code ported from KK2V1_1V12S1Beginner code
//* by Rolf Bakke and Steveis
//*
//* Ported to OpenAeroVTOL by David Thompson (C)2014
//***********************************************************

//************************************************************
//   Defines
//************************************************************

#define ACCEL_0_4G_SQ      2621.0f  // (0.40 * ACCSENSITIVITY) * (0.40 * ACCSENSITIVITY)  
#define ACCEL_0_85G_SQ    11837.0f  // (0.85 * ACCSENSITIVITY) * (0.85 * ACCSENSITIVITY)
#define ACCEL_1_15G_SQ    21668.0f  // (1.15 * ACCSENSITIVITY) * (1.15 * ACCSENSITIVITY)
#define ACCEL_1_6G_SQ     41943.0f  // (1.60 * ACCSENSITIVITY) * (1.60 * ACCSENSITIVITY)

#define GYROSENSRADIANS   0.017045f // Calculate factor for Gyros to report directly in rad/s
#define MAXDELTAANGLE     0.2618f   // Limit possible instantaneous change in angle to +/-15 degrees (720 deg/s)
#define SMALLANGLEFACTOR  0.66f     // Empirically calculated to produce exactly 20 at 20 degrees. Was 0.66

//************************************************************
//   Globals
//************************************************************

float vectorX = 0;  // Initialise the vector to point straight up
float vectorY = 0;
float vectorZ = 1;

float accelAnglePitch, accelAngleRoll, eulerAngleRoll, eulerAnglePitch;
float accelSmooth[NUMBEROFAXIS];           // Filtered accel data
float gyroPitchVC, gyroRollVC, gyroYawVC;
float intervalF;
float vectorA, vectorB;
float vectorNewA, vectorNewB;

int16_t angle[2];                 // Attitude in degrees - pitch and roll

// Software LPF conversion table      5Hz,  10Hz, 21Hz, 44Hz, 94Hz, 184Hz, 260Hz, None  
const float lpfLookup[8] PROGMEM = {16.43,  8.27, 4.18, 2.22, 1.30,  1.00,  1.00, 1.00};  // 500 Hz
// HJI const float lpfLookup[8] PROGMEM = {32.86, 16.54, 8.36, 4.43, 2.60,  1.93,  1.77, 1.00};  // 1000 Hz

//************************************************************
//
// Code
//
// Assume that all gyro (gyroADC[]) and acc signals (accADC[]) are 
// calibrated and have the appropriate zeros removed. At rest, only the 
// Z acc has a value, usually around +128, for the 1g earth gravity vector
//
// Notes:
// 1. KK code loads the MPU6050's ACCEL_XOUT data into AccY and ACCEL_YOUT into AccX (swapping X/Y axis)
// 2. In KK code GyroPitch gets GYRO_XOUT, GyroRoll gets GYRO_YOUT. GyroYaw gets GYRO_ZOUT.
//    This means that AccX(ACCEL_YOUT) and GyroPitch are the pitch components, and AccY(ACCEL_XOUT) and GyroRoll are the roll components 
//    In other words, AccX(actually ACCEL_YOUT) and GYRO_XOUT are cludged to work together, which looks right, but by convention is wrong.
//    By IMU convention, rotation around a gyro's X axis, for example, will cause the acc's Y axis to change.
// 3. Of course, OpenAero code does exactly the same cludge, lol. But in this case, pitch and roll *gyro* data are swapped. 
//    The result is the same, pairing opposing (X/Y/Z) axis together to the same (R/P/Y) axis name.
//
//    Actual hardware   KK2 code      OpenAero2 code
//    ACCEL_XOUT        = AccY*       = accADC[ROLL]
//    ACCEL_YOUT        = AccX*       = accADC[PITCH]
//    ACCEL_ZOUT        = AccZ        = accADC[YAW/Z]
//    GYRO_XOUT         = GyroPitch   = gyroADC[PITCH]*
//    GYRO_YOUT         = GyroRoll    = gyroADC[ROLL]*
//    GYRO_ZOUT         = GyroYaw     = gyroADC[YAW]
//
//    * = swapped axis
//
//************************************************************

void imuUpdate(float period)
{
  float     tempF, accelAdcF;
  int8_t    axis;
  uint32_t  rollSq, pitchSq, yawSq;
  uint32_t  accelMag = 0;
    
  intervalF = period;

  //************************************************************
  // Acc LPF
  //************************************************************  

  memcpy_P(&tempF, &lpfLookup[config.accLPF], sizeof(float));
  
  // Smooth Acc signals - note that accSmooth is in [ROLL, PITCH, YAW] order
  for (axis = 0; axis < NUMBEROFAXIS; axis++)
  {
    accelAdcF = accelAdc[axis]; // Promote
    
    // Acc LPF
    if (config.accLPF != NOFILTER)
    {
      accelSmooth[axis] = ((accelSmooth[axis] * (tempF - 1.0f)) - accelAdcF) / tempF;
    }
    else
    {
      // Use raw accADC[axis] as source for acc values when filter off
      accelSmooth[axis] =  -accelAdcF;
    }
  }
  
  // Z Acc LPF for long-term zero
  //accVertZerof = ((accVertZerof * (ZACCLPF - 1.0f)) - accVertf) / ZACCLPF; // Heavy filter
    
  // Add correction data to gyro inputs based on difference between Euler angles and acc angles
  accelAngleRoll  = accelSmooth[ROLL]  * SMALLANGLEFACTOR;    // KK2 - AccYfilter
  accelAnglePitch = accelSmooth[PITCH] * SMALLANGLEFACTOR;

  // Alter the gyro sources to the IMU as required.
  // Using gyroADCalt[] always assures that the right gyros are associated with the IMU
  gyroRollVC  = gyroAdcAlt[ROLL];
  gyroPitchVC = gyroAdcAlt[PITCH];
  gyroYawVC   = gyroAdcAlt[YAW];

  // Calculate acceleration magnitude.
  rollSq   = (accelAdc[ROLL]  * accelAdc[ROLL]);
  pitchSq  = (accelAdc[PITCH] * accelAdc[PITCH]);
  yawSq    = (accelAdc[YAW]   * accelAdc[YAW]);
  accelMag = rollSq + pitchSq + yawSq;
  
  // Add acc correction if inside local acceleration bounds and not inverted according to VectorZ
  // NB: new dual autolevel code needs acc correction at least temporarily when switching profiles.
  // This is actually a kind of Complementary Filter
  //if ((AccMag > acc_0_85G_SQ) && (AccMag < acc_1_15G_SQ) && (VectorZ > 0.5)) // Original code
  
  // New test code - only adjust when in acc mag limits and when upright or dual AL code
  if (((accelMag > ACCEL_0_85G_SQ) && (accelMag < ACCEL_1_15G_SQ) && (vectorZ > 0.5) && (config.p1Reference == NO_ORIENT)) || // Same as always when "Same" 
      ((accelMag > ACCEL_0_4G_SQ)  && (accelMag < ACCEL_1_6G_SQ)  && (config.p1Reference != NO_ORIENT))) 
  {
    // Default Config.CF_factor is 6 (1 - 10 = 10% to 100%, 6 = 60%)
    tempF = (eulerAngleRoll - accelAngleRoll) / 10;
    tempF = tempF * (12 - config.cfFactor); 
    gyroRollVC = gyroRollVC + tempF;
    
    tempF = (eulerAnglePitch - accelAnglePitch) / 10;
    tempF = tempF * (12 - config.cfFactor);
    gyroPitchVC = gyroPitchVC + tempF;
  }

  // Rotate up-direction 3D vector with gyro inputs
  rotate3dVector(intervalF);
  extractEulerAngles();
  
  // Upscale to 0.01 degrees resolution and copy to angle[] for display
  angle[ROLL]  = (int16_t)(eulerAngleRoll  * -100);
  angle[PITCH] = (int16_t)(eulerAnglePitch * -100);
}

void rotate3dVector(float intervalf)
{
  float theta;
  
  // Rotate around X axis (pitch)
  theta = thetaScale(gyroPitchVC, intervalF);
  vectorA = vectorY;
  vectorB = vectorZ;
  rotateVector(theta);
  vectorY = vectorNewA;
  vectorZ = vectorNewB;

  // Rotate around Y axis (roll)
  theta = thetaScale (gyroRollVC, intervalF);
  vectorA = vectorX;
  vectorB = vectorZ;
  rotateVector(theta);
  vectorX = vectorNewA;
  vectorZ = vectorNewB;

  // Rotate around Z axis (yaw)
  theta = thetaScale(gyroYawVC, intervalF);
  vectorA = vectorX;
  vectorB = vectorY;
  rotateVector(theta);
  vectorX = vectorNewA;
  vectorY = vectorNewB;
}

void rotateVector(float angle)
{
  vectorNewA = vectorA * smallCos(angle)  - vectorB * smallSine(angle);
  vectorNewB = vectorA * smallSine(angle) + vectorB * smallCos(angle);
}

float thetaScale(float gyro, float intervalf)
{
  float theta;
  
  // intervalf = time in seconds since last measurement
  // GYROSENSRADIANS = conversion from raw gyro data to rad/s
  // theta = actual number of radians moved

  theta = (gyro * GYROSENSRADIANS * intervalf);
  
  // The sin() and cos() functions don't appreciate large 
  // input values. Limit the input values to +/-15 degrees. 
  
  if (theta > MAXDELTAANGLE)
  {
    theta = MAXDELTAANGLE;
  }
  
  if (theta < -MAXDELTAANGLE)
  {
    theta = -MAXDELTAANGLE;
  }
  
  return theta;
}

// Small angle approximations of Sine, Cosine
// NB:  These *only* work for small input values.
//    Larger values will produce fatal results
float smallSine(float angle)
{
  // sin(angle) = angle
  return angle;
}

float smallCos(float angle)
{
  // cos(angle) = (1 - (angle^2 / 2))
  float temp;
  
  temp = (angle * angle) / 2;
  temp = 1 - temp;

  return temp;
}

void extractEulerAngles(void)
{
  eulerAngleRoll = ext2(vectorX);
  eulerAnglePitch = ext2(vectorY);
}

float ext2(float vector)
{
  float temp;
  
  // Rough translation to Euler angles
  temp = vector * 90;

  // Change 0-90-0 to 0-90-180 so that
  // swap happens at 100% inverted
  if (vectorZ < 0)
  {
    // CW rotations
    if (temp > 0)
    {
      temp = 180 - temp;
    }
    // CCW rotations
    else
    {
      temp = -180 - temp;
    }
  }

  return (temp);
}

void resetIMU(void)
{
  // Initialise the vector to point straight up
  vectorX = 0;
  vectorY = 0;
  vectorZ = 1;
  
  // Initialise internal vectors and attitude 
  vectorA = 0;
  vectorB = 0;
  eulerAngleRoll = 0;
  eulerAnglePitch = 0;
}
