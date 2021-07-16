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

// ***********************************************************
// * ROLL, PITCH, YAW mapping for alternate orientation modes
// ***********************************************************

// This is the translation required to return axis data in ROLL, PITCH, YAW order
const int8_t accelRpyOrder[NUMBEROFORIENTS][NUMBEROFAXIS] PROGMEM = 
{
  //ROLL, PITCH,  YAW
  { ROLL, PITCH,  YAW},  // Up/Back     (Normal)       [Position 2] -> [Position 3]
  {PITCH,  ROLL,  YAW},  // Up/Left
  { ROLL, PITCH,  YAW},  // Up/Front    (Aft)          (Backwards-compatibility. Not for tail-sitters)
  {PITCH,  ROLL,  YAW},  // Up/Right    (Sideways)     (Backwards-compatibility. Not for tail-sitters)
  
  { ROLL,  YAW, PITCH},  // Back/Down   (PitchUp)      [Position 1] -> [Position 2]
  {PITCH,  YAW,  ROLL},  // Back/Left
  { ROLL,  YAW, PITCH},  // Back/Up
  {PITCH,  YAW,  ROLL},  // Back/Right
  
  { ROLL, PITCH,  YAW},  // Down/Back   (Upside down)  (Backwards-compatibility. Not for tail-sitters)
  {PITCH,  ROLL,  YAW},  // Down/Right
  { ROLL, PITCH,  YAW},  // Down/Front  [Position 4] -> [Position 1]
  {PITCH,  ROLL,  YAW},  // Down/Left
    
  { ROLL,  YAW, PITCH},  // Front/Down
  {PITCH,  YAW,  ROLL},  // Front/Right
  { ROLL,  YAW, PITCH},  // Front/Up    [Position 3] -> [Position 4]
  {PITCH,  YAW,  ROLL},  // Front/Left
    
  {  YAW,  ROLL, PITCH},  // Left/Down  [Position 5] -> [Position 7]
  {  YAW, PITCH,  ROLL},  // Left/Front
  {  YAW,  ROLL, PITCH},  // Left/Up
  {  YAW, PITCH,  ROLL},  // Left/Back  [Position 7]   (destination for Position 5 only)
    
  {  YAW,  ROLL, PITCH},  // Right/Down (Vertical)     [Position 6] -> [Position 8]
  {  YAW, PITCH,  ROLL},  // Right/Back [Position 8]   (destination for Position 6 only)
  {  YAW,  ROLL, PITCH},  // Right/Up
  {  YAW, PITCH,  ROLL},  // Right/Front
};

// These are the polarities to return them to the default
const int8_t accelPol[NUMBEROFORIENTS][NUMBEROFAXIS] PROGMEM =
{
  // ROLL, PITCH, YAW
  {-1,-1, 1},  // Up/Back (Normal)
  { 1,-1, 1},  // Up/Left
  { 1, 1, 1},  // Up/Front (Aft)
  {-1, 1, 1},  // Up/Right (Sideways)
  
  {-1,-1,-1},  // Back/Down (PitchUp)
  { 1,-1,-1},  // Back/Left
  { 1,-1, 1},  // Back/Up
  {-1,-1, 1},  // Back/Right
  
  { 1,-1,-1},  // Down/Back (Upside down)
  {-1,-1,-1},  // Down/Right
  {-1, 1,-1},  // Down/Front
  { 1, 1,-1},  // Down/Left
  
  { 1, 1,-1},  // Front/Down
  {-1, 1,-1},  // Front/Right
  {-1, 1, 1},  // Front/Up
  { 1, 1, 1},  // Front/Left
  
  { 1,-1,-1},  // Left/Down
  { 1, 1,-1},  // Left/Front
  { 1, 1, 1},  // Left/Up
  { 1,-1, 1},  // Left/Back
  
  {-1, 1,-1},  // Right/Down (Vertical)
  {-1,-1,-1},  // Right/Back
  {-1,-1, 1},  // Right/Up
  {-1, 1, 1},  // Right/Front
};

//************************************************************
// Code
//************************************************************

int16_t accelAdcP1[NUMBEROFAXIS];  // Holds Acc ADC values - always in RPY order (P1)
int16_t accelAdcP2[NUMBEROFAXIS];  // Holds Acc ADC values - always in RPY order (P2)

void readAccels()
{
  uint8_t i;
  int16_t temp1, temp2;

  getRawAccels();  // Updates accADC_P1[] and accADC_P2[] (RPY)

  // P1
  // Use default Config.AccZero for Acc-Z if inverse calibration not done yet
  // Actual zero is held in Config.AccZeroNormZ waiting for inverse calibration
  if (!(config.mainFlags & (1 << INV_CAL_DONE_P1)))
  {
    config.accelZeroP1[YAW] = 0;
  }
  // If inverted cal done, config.accZeroNormZ and config.accZeroDiff have valid values
  else
  {
    config.accelZeroP1[YAW] = config.accelZeroNormZP1 - config.accelZeroDiffP1;
  }
  
  // P2
  if (!(config.mainFlags & (1 << INV_CAL_DONE_P2)))
  {
    config.accelZeroP2[YAW] = 0;
  }
  // If inverted cal done, config.accZeroNormZ and config.accZeroDiff have valid values
  else
  {
    config.accelZeroP2[YAW] = config.accelZeroNormZP2 - config.accelZeroDiffP2;
  }

  // Roll and Pitch are handled normally
  for (i = 0; i < (NUMBEROFAXIS - 1); i++)
  {
    // Only need to do this if the orientations differ
    if (config.p1Reference != NO_ORIENT)
    {
      // Change polarity - use the zeros from the appropriate calibrate
      temp1 = ((accelAdcP1[i] - config.accelZeroP1[i]) * (int8_t)pgm_read_byte(&accelPol[config.orientationP1][i]));
      temp2 = ((accelAdcP2[i] - config.accelZeroP2[i]) * (int8_t)pgm_read_byte(&accelPol[config.orientationP2][i]));
      
      // Get P1 value
      temp1 = scale32(temp1, (100 - transition));

      // Get P2 value
      temp2 = scale32(temp2, transition);

      // Sum the two values
      accelAdc[i] = temp1 + temp2;
    }
    else
    {
      accelAdc[i] = ((accelAdcP2[i] - config.accelZeroP2[i]) * (int8_t)pgm_read_byte(&accelPol[config.orientationP2][i]));
    }
  }
  
  // Z -axis requires special handling as the zeros are already polarity corrected
  // Only need to do this if the orientations differ
  if (config.p1Reference != NO_ORIENT)
  {
    // Change polarity - use the zeros from the appropriate calibrate
    temp1 = ((accelAdcP1[YAW] * (int8_t)pgm_read_byte(&accelPol[config.orientationP1][YAW]) - config.accelZeroP1[YAW]));
    temp2 = ((accelAdcP2[YAW] * (int8_t)pgm_read_byte(&accelPol[config.orientationP2][YAW]) - config.accelZeroP2[YAW]));
      
    // Get P1 value
    temp1 = scale32(temp1, (100 - transition));

    // Get P2 value
    temp2 = scale32(temp2, transition);

    // Sum the two values
    accelAdc[YAW] = temp1 + temp2;
  }
  else
  {
    accelAdc[YAW] = ((accelAdcP2[YAW] * (int8_t)pgm_read_byte(&accelPol[config.orientationP2][YAW]) - config.accelZeroP2[YAW]));
  }
    
  // Recalculate current accVertf using filtered acc value
  // Note that accSmooth[YAW] is already zeroed around 1G so we have to re-add 
  // the zero back here so that Config.AccZeroNormZ subtracts the correct amount
  // accVertf = accSmooth[YAW] + (config.accZeroNormZ - config.accZero[YAW]);
  
  // Note also that accSmooth[] has already got the correct acc orientations, 
  // so only needs the zeroing value merged from one to the other.

  // Only need to do this if the orientations differ
  if (config.p1Reference != NO_ORIENT)
  {
    // Calculate the correct Z-axis data based on the orientation
    temp1 = accelSmooth[YAW] + (config.accelZeroNormZP1 - config.accelZeroP1[YAW]); 
    temp2 = accelSmooth[YAW] + (config.accelZeroNormZP2 - config.accelZeroP2[YAW]); 
  
    // Merge with transition
    temp1 = scale32(temp1, (100 - transition));
    temp2 = scale32(temp2, transition);
   
    accelVertF = (float)temp1 + temp2;
  }
  // Just use the P2 value
  else
  {
    // Calculate the correct Z-axis data based on the orientation
    accelVertF = accelSmooth[YAW] + (float)(config.accelZeroNormZP2 - config.accelZeroP2[YAW]);   
  }
}

//***************************************************************
// Fill accADC with RPY data appropriate to the board orientation
// Nice as it would be, we cannot remove zeros here as this is the
// routine used by the zeroing calibration routine. Chicken | Egg.
// We also cannot merge P1 and P2 here as each have their own zeros.
//***************************************************************

void getRawAccels(void)
{
  int16_t accelAdcRaw[NUMBEROFAXIS];
  uint8_t i;

  // Reassemble data into accADC array and down sample to reduce resolution and noise.
  // This notation is true to the chip, but not the board orientation.
  accelAdcRaw[ROLL]  =  ax;
  accelAdcRaw[PITCH] = -ay;
  accelAdcRaw[YAW]   =  az;

  // Reorient the data as per the board orientation 
  for (i = 0; i < NUMBEROFAXIS; i++)
  {
    // Rearrange the sensors for both orientations
    accelAdcP1[i] = accelAdcRaw[(int8_t)pgm_read_byte(&accelRpyOrder[config.orientationP1][i])] >> 6;
    accelAdcP2[i] = accelAdcRaw[(int8_t)pgm_read_byte(&accelRpyOrder[config.orientationP2][i])] >> 6;
  }
}

//***************************************************************
// Calibration routines
// For V1.2 onwards we have to be able to calibrate both
// orientations when there are more than one.
//***************************************************************

void calibrateAcc(int8_t type)
{
  uint8_t i;
  int16_t accelZero[NUMBEROFAXIS] = {0,0,0};  // Used for calibrating Accs on ground

  // Calibrate acc
  // P2
  if (type == NORMAL)
  {
    // Work out which orientation we are calibrating.
    // Only need to do this if the orientations differ.
    // Just do P2 if orientations the same.
    // Will not save new calibration when different and not firmly in P1 or p2.
    if ((transition > 95) || (config.p1Reference == NO_ORIENT))
    {
      // Get average zero value (over 32 readings)
      for (i = 0; i < 32; i++)
      {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        getRawAccels();  // Updates accADC_P1[] and accADC_P2[] (RPY)
        accelZero[ROLL]  += accelAdcP2[ROLL];
        accelZero[PITCH] += accelAdcP2[PITCH];
        accelZero[YAW]   += accelAdcP2[YAW];
        delay(10);       // Get a better acc average over time
      }
      
      // Average
      for (i = 0; i < NUMBEROFAXIS; i++)    // For selected axis in RPY order
      {
        // Round and divide by 32
        accelZero[i] = ((accelZero[i] + 16) >> 5);
      }

      // Reset zeros to normal cal
      config.accelZeroP2[ROLL]  = accelZero[ROLL];
      config.accelZeroP2[PITCH] = accelZero[PITCH];
      config.accelZeroNormZP2   = accelZero[YAW];
      
      // Correct polarity of AccZeroNormZ as per orientation
      config.accelZeroNormZP2 *= (int8_t)pgm_read_byte(&accelPol[config.orientationP2][YAW]);
      
      // Flag that normal cal done
      config.mainFlags |= (1 << NORMAL_CAL_DONE_P2);
      
      // Save new calibration and flash LED for confirmation
      saveConfigToEEPROM();
      LED_ON;
      delay(500);
      LED_OFF;
    }
    // P1
    else if (transition <= 5)
    {
      // Get average zero value (over 32 readings)
      for (i = 0; i < 32; i++)
      {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        getRawAccels();  // Updates accADC_P1[] and accADC_P2[] (RPY)
        accelZero[ROLL]  += accelAdcP1[ROLL];
        accelZero[PITCH] += accelAdcP1[PITCH];
        accelZero[YAW]   += accelAdcP1[YAW];
        delay(10);       // Get a better acc average over time
      }
      
      // Average
      for (i = 0; i < NUMBEROFAXIS; i++)    // For selected axis in RPY order
      {
        // Round and divide by 32
        accelZero[i] = ((accelZero[i] + 16) >> 5);
      }

      // Reset zeros to normal cal
      config.accelZeroP1[ROLL]  = accelZero[ROLL];
      config.accelZeroP1[PITCH] = accelZero[PITCH];
      config.accelZeroNormZP1   = accelZero[YAW];
      
      // Correct polarity of AccZeroNormZ as per orientation
      config.accelZeroNormZP1 *= (int8_t)pgm_read_byte(&accelPol[config.orientationP1][YAW]);
      
      // Flag that normal cal done
      config.mainFlags |= (1 << NORMAL_CAL_DONE_P1);

      // Save new calibration and flash LED for confirmation
      saveConfigToEEPROM();
      LED_ON;
      delay(500);
      LED_OFF;
    }
  }

  else
  // Calibrate inverted acc
  {
    // P2 or same
    if ((transition > 95) || (config.p1Reference == NO_ORIENT))
    {
      // Only update the inverted cal value if preceded by a normal calibration
      if (config.mainFlags & (1 << NORMAL_CAL_DONE_P2))
      {
        // Get average zero value (over 32 readings)
        config.accelZeroInvZP2 = 0;

        for (i = 0; i < 32; i++)
        {
          mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
          getRawAccels();  // Updates accADC_P1[] and accADC_P2[] (RPY)
          config.accelZeroInvZP2 += accelAdcP2[YAW];
          delay(10);       // Get a better acc average over time
        }

        // Round and divide by 32
        config.accelZeroInvZP2 = ((config.accelZeroInvZP2 + 16) >> 5);    // Inverted zero point
        
        // Correct polarity of AccZeroInvZ as per orientation
        config.accelZeroInvZP2 *= (int8_t)pgm_read_byte(&accelPol[config.orientationP2][YAW]);

        // Test if board is actually inverted relative to board orientation.
        if (config.accelZeroInvZP2 < 0)
        {
          // Reset zero to halfway between min and max Z
          config.accelZeroDiffP2 = ((config.accelZeroNormZP2 - config.accelZeroInvZP2) >> 1);
          
           // Config.AccZero_P2[YAW] is now half-way in between
          config.accelZeroP2[YAW] = config.accelZeroNormZP2 - config.accelZeroDiffP2;

          // Flag that inverted cal done
          config.mainFlags |= (1 << INV_CAL_DONE_P2);

          // Save new calibration and flash LED for confirmation
          saveConfigToEEPROM();
          LED_ON;
          delay(500);
          LED_OFF;
          
          // Chirp as well. The LED might be hard to see.
          // HJI menu_beep(5);
        }
      }
    } // Orientation-specific code
    
    // P1
    else 
    {
      // Only update the inverted cal value if preceded by a normal calibration
      if (config.mainFlags & (1 << NORMAL_CAL_DONE_P1))
      {
        // Get average zero value (over 32 readings)
        config.accelZeroInvZP1 = 0;

        for (i = 0; i < 32; i++)
        {
          mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
          getRawAccels();  // Updates accADC_P1[] and accADC_P2[] (RPY)
          config.accelZeroInvZP1 += accelAdcP1[YAW];
          delay(10);       // Get a better acc average over time
        }

        // Round and divide by 32
        config.accelZeroInvZP1 = ((config.accelZeroInvZP1 + 16) >> 5);    // Inverted zero point
      
        // Correct polarity of AccZeroInvZ as per orientation
        config.accelZeroInvZP1 *= (int8_t)pgm_read_byte(&accelPol[config.orientationP1][YAW]);

        // Test if board is actually inverted relative to board orientation.
        if (config.accelZeroInvZP1 < 0)
        {
          // Reset zero to halfway between min and max Z
          config.accelZeroDiffP1 = ((config.accelZeroNormZP1 - config.accelZeroInvZP1) >> 1);
          
          // config.accZeroP1[YAW] is now half-way in between
          config.accelZeroP1[YAW] = config.accelZeroNormZP1 - config.accelZeroDiffP1;

          // Flag that inverted cal done
          config.mainFlags |= (1 << INV_CAL_DONE_P1);

          // Save new calibration and flash LED for confirmation
          saveConfigToEEPROM();
          LED_ON;
          delay(500);
          LED_OFF;
        
          // Chirp as well. The LED might be hard to see.
          // HJI menu_beep(5);
        }
      }     
    }

  } // Calibrate inverted acc
}
