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

// Throttle volume curves
// Why 101 steps? Well, both 0% and 100% transition values are valid...

const int8_t SIN[101] PROGMEM =
{ 0, 2, 3, 5, 6, 8, 10, 11, 13, 14,
  16, 17, 19, 20, 22, 23, 25, 26, 28, 29,
  31, 32, 34, 35, 37, 38, 40, 41, 43, 44,
  45, 47, 48, 50, 51, 52, 54, 55, 56, 58,
  59, 60, 61, 63, 64, 65, 66, 67, 68, 70,
  71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
  81, 82, 83, 84, 84, 85, 86, 87, 88, 88,
  89, 90, 90, 91, 92, 92, 93, 94, 94, 95,
  95, 96, 96, 96, 97, 97, 98, 98, 98, 99,
  99, 99, 99, 99, 100, 100, 100, 100, 100, 100,
  100
};

const int8_t SQRTSIN[101] PROGMEM =
{ 0, 13, 18, 22, 25, 28, 31, 33, 35, 38,
  40, 41, 43, 45, 47, 48, 50, 51, 53, 54,
  56, 57, 58, 59, 61, 62, 63, 64, 65, 66,
  67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
  77, 77, 78, 79, 80, 81, 81, 82, 83, 83,
  84, 85, 85, 86, 87, 87, 88, 88, 89, 89,
  90, 90, 91, 91, 92, 92, 93, 93, 94, 94,
  94, 95, 95, 95, 96, 96, 96, 97, 97, 97,
  98, 98, 98, 98, 98, 99, 99, 99, 99, 99,
  99, 99, 100, 100, 100, 100, 100, 100, 100, 100,
  100
};

//************************************************************
// Globals
//************************************************************

int16_t p1CurveC = 0;  // Generic curve C
int16_t p2CurveC = 0;  // Generic curve C
int16_t p1CurveD = 0;  // Generic curve D
int16_t p2CurveD = 0;  // Generic curve D

//************************************************************
// Code
//************************************************************

void processMixer(void)
{
  uint8_t i = 0;
  int16_t p1Solution = 0;
  int16_t p2Solution = 0;

  int16_t temp1 = 0;
  int16_t temp2 = 0;
  int16_t temp3 = 0;
  int16_t step1 = 0;
  int16_t monoThrottle = 0;

  int32_t e32Temp1 = 0;
  int32_t e32Temp2 = 0;
  int32_t e32Temp3 = 0;
  int32_t e32Step1 = 0;

  // Curve outputs
  int16_t p1Throttle   = 0;  // P1 Throttle curve
  int16_t p2Throttle   = 0;  // P2 Throttle curve
  int16_t p1Collective = 0;  // P1 Collective curve
  int16_t p2Collective = 0;  // P2 Collective curve

  int8_t  p1AccelRollVolumeSource = 0;
  int8_t  p1GyroRollVolumeSource  = 0;
  int8_t  p1GyroYawVolumeSource   = 0;

  // Process curves
  p1Throttle   = processCurve(P1_THR_CURVE,  MONOPOLAR, monopolarThrottle);
  p2Throttle   = processCurve(P2_THR_CURVE,  MONOPOLAR, monopolarThrottle);
  p1Collective = processCurve(P1_COLL_CURVE, BIPOLAR,   rcInputs[THROTTLE]);
  p2Collective = processCurve(P2_COLL_CURVE, BIPOLAR,   rcInputs[THROTTLE]);

  // Copy the universal mixer inputs to an array for easy indexing - acc data is from accSmooth, increased to reasonable rates
  temp1 = (int16_t)accelSmooth[ROLL] << 3;
  temp2 = (int16_t)accelSmooth[PITCH] << 3;

  // THROTTLE, CURVE A, CURVE B, COLLECTIVE, THROTTLE, AILERON, ELEVATOR, RUDDER, GEAR, AUX1, AUX2, AUX3, ROLLGYRO, PITCHGYO, YAWGYRO, ACCSMOOTH, PITCHSMOOTH, ROLLACC, PITCHACC, AccZ, NONE
  int16_t universalP1[NUMBEROFSOURCES] =
  { p1Throttle, p1CurveC, p1CurveD, p1Collective, rcInputs[THROTTLE], rcInputs[AILERON], rcInputs[ELEVATOR], rcInputs[RUDDER], rcInputs[GEAR], rcInputs[AUX1], rcInputs[AUX2], rcInputs[AUX3],
    pidGyros[P1][ROLL], pidGyros[P1][PITCH], pidGyros[P1][YAW], temp1, temp2, pidAccels[P1][ROLL], pidAccels[P1][PITCH], pidAccels[P1][YAW], 0
  };

  int16_t universalP2[NUMBEROFSOURCES] =
  { p2Throttle, p2CurveC, p2CurveD, p2Collective, rcInputs[THROTTLE], rcInputs[AILERON], rcInputs[ELEVATOR], rcInputs[RUDDER], rcInputs[GEAR], rcInputs[AUX1], rcInputs[AUX2], rcInputs[AUX3],
    pidGyros[P2][ROLL], pidGyros[P2][PITCH], pidGyros[P2][YAW], temp1, temp2, pidAccels[P2][ROLL], pidAccels[P2][PITCH], pidAccels[P2][YAW], 0
  };

  //************************************************************
  // Generic curves
  //************************************************************

  // Only process generic curves if they have a source selected
  if (config.curve[GEN_CURVE_C].channel != NOMIX)
  {
    p1CurveC = processCurve(GEN_CURVE_C, BIPOLAR, universalP1[config.curve[GEN_CURVE_C].channel]);
    p2CurveC = processCurve(GEN_CURVE_C, BIPOLAR, universalP2[config.curve[GEN_CURVE_C].channel]);
  }
  else
  {
    p1CurveC = 0;
    p2CurveC = 0;
  }

  if (config.curve[GEN_CURVE_D].channel != NOMIX)
  {
    p1CurveD = processCurve(GEN_CURVE_D, BIPOLAR, universalP1[config.curve[GEN_CURVE_D].channel]);
    p2CurveD = processCurve(GEN_CURVE_D, BIPOLAR, universalP2[config.curve[GEN_CURVE_D].channel]);
  }
  else
  {
    p1CurveD = 0;
    p2CurveD = 0;
  }

  //************************************************************
  // Main mix loop - sensors, RC inputs and other channels
  //************************************************************

  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    //************************************************************
    // Zero each channel value to start
    //************************************************************

    p1Solution = 0;
    p2Solution = 0;

    //************************************************************
    // Mix in gyros
    //************************************************************

    // If the user wants earth reference for tail-sitter hover, swap the related stick sources.
    // The secret is understanding WHICH STICK is controlling movement on the AXIS in the selected REFERENCE
    // Only need to do this if the orientations differ
    if (config.p1Reference != NO_ORIENT)
    {
      // EARTH-Referenced tail-sitter
      if (config.p1Reference == EARTH)
      {
        p1AccelRollVolumeSource = config.channel[i].p1AileronVolume;
        p1GyroRollVolumeSource  = config.channel[i].p1AileronVolume;  // These are always the same
        p1GyroYawVolumeSource   = config.channel[i].p1RudderVolume;   // These are always the same
      }
      // MODEL-Referenced tail-sitter
      else
      {
        p1AccelRollVolumeSource = config.channel[i].p1RudderVolume;
        p1GyroRollVolumeSource  = config.channel[i].p1AileronVolume;
        p1GyroYawVolumeSource   = config.channel[i].p1RudderVolume;
      }
    }
    // Normal case
    else
    {
      p1AccelRollVolumeSource = config.channel[i].p1AileronVolume;
      p1GyroRollVolumeSource  = config.channel[i].p1AileronVolume;
      p1GyroYawVolumeSource   = config.channel[i].p1RudderVolume;
    }

    // P1 gyros
    if (transition < 100)
    {
      switch (config.channel[i].p1RollGyro)
      {
        case OFF:
          break;
        case ON:
          if (p1GyroRollVolumeSource < 0 )
          {
            p1Solution = p1Solution + pidGyros[P1][ROLL];  // Reverse if volume negative
          }
          else
          {
            p1Solution = p1Solution - pidGyros[P1][ROLL];
          }
          break;
        case SCALE:
          p1Solution = p1Solution - scale32(pidGyros[P1][ROLL], p1GyroRollVolumeSource * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p1PitchGyro)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p1ElevatorVolume < 0 )
          {
            p1Solution = p1Solution - pidGyros[P1][PITCH];  // Reverse if volume negative
          }
          else
          {
            p1Solution = p1Solution + pidGyros[P1][PITCH];
          }
          break;
        case SCALE:
          p1Solution = p1Solution + scale32(pidGyros[P1][PITCH], config.channel[i].p1ElevatorVolume * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p1YawGyro)
      {
        case OFF:
          break;
        case ON:
          if (p1GyroYawVolumeSource < 0 )
          {
            p1Solution = p1Solution - pidGyros[P1][YAW];  // Reverse if volume negative
          }
          else
          {
            p1Solution = p1Solution + pidGyros[P1][YAW];
          }
          break;
        case SCALE:
          p1Solution = p1Solution + scale32(pidGyros[P1][YAW], p1GyroYawVolumeSource * 5);
          break;
        default:
          break;
      }
    }

    // P2 gyros
    if (transition > 0)
    {
      switch (config.channel[i].p2RollGyro)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p2AileronVolume < 0 )
          {
            p2Solution = p2Solution + pidGyros[P2][ROLL];  // Reverse if volume negative
          }
          else
          {
            p2Solution = p2Solution - pidGyros[P2][ROLL];
          }
          break;
        case SCALE:
          p2Solution = p2Solution - scale32(pidGyros[P2][ROLL], config.channel[i].p2AileronVolume * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p2PitchGyro)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p2ElevatorVolume < 0 )
          {
            p2Solution = p2Solution - pidGyros[P2][PITCH];  // Reverse if volume negative
          }
          else
          {
            p2Solution = p2Solution + pidGyros[P2][PITCH];
          }
          break;
        case SCALE:
          p2Solution = p2Solution + scale32(pidGyros[P2][PITCH], config.channel[i].p2ElevatorVolume * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p2YawGyro)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p2RudderVolume < 0 )
          {
            p2Solution = p2Solution - pidGyros[P2][YAW];  // Reverse if volume negative
          }
          else
          {
            p2Solution = p2Solution + pidGyros[P2][YAW];
          }
          break;
        case SCALE:
          p2Solution = p2Solution + scale32(pidGyros[P2][YAW], config.channel[i].p2RudderVolume * 5);
          break;
        default:
          break;
      }
    }

    //************************************************************
    // Mix in accelerometers
    //************************************************************
    // P1
    if (transition < 100)
    {
      switch (config.channel[i].p1RollAccel)
      {
        case OFF:
          break;
        case ON:
          if (p1AccelRollVolumeSource < 0 )
          {
            p1Solution = p1Solution + pidAccels[P1][ROLL];  // Reverse if volume negative
          }
          else
          {
            p1Solution = p1Solution - pidAccels[P1][ROLL];  // or simply add
          }
          break;
        case SCALE:
          p1Solution = p1Solution - scale32(pidAccels[P1][ROLL], p1AccelRollVolumeSource * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p1PitchAccel)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p1ElevatorVolume < 0 )
          {
            p1Solution = p1Solution - pidAccels[P1][PITCH];  // Reverse if volume negative
          }
          else
          {
            p1Solution = p1Solution + pidAccels[P1][PITCH];
          }
          break;
        case SCALE:
          p1Solution = p1Solution + scale32(pidAccels[P1][PITCH], config.channel[i].p1ElevatorVolume * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p1ZDeltaAccel)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p1ThrottleVolume < 0 )
          {
            p1Solution = p1Solution + pidAccels[P1][YAW];  // Reverse if volume negative
          }
          else
          {
            p1Solution = p1Solution - pidAccels[P1][YAW];
          }
          break;
        case SCALE:
          p1Solution = p1Solution - scale32(pidAccels[P1][YAW], config.channel[i].p1ThrottleVolume * 5);
          break;
        default:
          break;
      }
    }

    // P2
    if (transition > 0)
    {
      switch (config.channel[i].p2RollAccel)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p2AileronVolume < 0 )
          {
            p2Solution = p2Solution + pidAccels[P2][ROLL];  // Reverse if volume negative
          }
          else
          {
            p2Solution = p2Solution - pidAccels[P2][ROLL];  // or simply add
          }
          break;
        case SCALE:
          p2Solution = p2Solution - scale32(pidAccels[P2][ROLL], config.channel[i].p2AileronVolume * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p2PitchAccel)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p2ElevatorVolume < 0 )
          {
            p2Solution = p2Solution - pidAccels[P2][PITCH];  // Reverse if volume negative
          }
          else
          {
            p2Solution = p2Solution + pidAccels[P2][PITCH];
          }
          break;
        case SCALE:
          p2Solution = p2Solution + scale32(pidAccels[P2][PITCH], config.channel[i].p2ElevatorVolume * 5);
          break;
        default:
          break;
      }

      switch (config.channel[i].p2ZDeltaAccel)
      {
        case OFF:
          break;
        case ON:
          if (config.channel[i].p2ThrottleVolume < 0 )
          {
            p2Solution = p2Solution + pidAccels[P2][YAW];  // Reverse if volume negative
          }
          else
          {
            p2Solution = p2Solution - pidAccels[P2][YAW];
          }
          break;
        case SCALE:
          p2Solution = p2Solution - scale32(pidAccels[P2][YAW], config.channel[i].p2ThrottleVolume * 5);
          break;
        default:
          break;
      }
    }

    //************************************************************
    // Process mixers
    //************************************************************

    // Mix in other outputs here (P1)
    if (transition < 100)
    {
      // Mix in dedicated RC sources - aileron, elevator and rudder
      if (config.channel[i].p1AileronVolume != 0)  // Mix in dedicated aileron
      {
        temp2 = scale32(rcInputs[AILERON], config.channel[i].p1AileronVolume);
        p1Solution = p1Solution + temp2;
      }
      if (config.channel[i].p1ElevatorVolume != 0)  // Mix in dedicated elevator
      {
        temp2 = scale32(rcInputs[ELEVATOR], config.channel[i].p1ElevatorVolume);
        p1Solution = p1Solution + temp2;
      }
      if (config.channel[i].p1RudderVolume != 0)  // Mix in dedicated rudder
      {
        temp2 = scale32(rcInputs[RUDDER], config.channel[i].p1RudderVolume);
        p1Solution = p1Solution + temp2;
      }

      // Other sources
      if ((config.channel[i].p1SourceAVolume != 0) && (config.channel[i].p1SourceA != NOMIX))  // Mix in first extra source
      {
        temp2 = universalP1[config.channel[i].p1SourceA];
        temp2 = scale32(temp2, config.channel[i].p1SourceAVolume);
        p1Solution = p1Solution + temp2;
      }
      if ((config.channel[i].p1SourceBVolume != 0) && (config.channel[i].p1SourceB != NOMIX))  // Mix in second extra source
      {
        temp2 = universalP1[config.channel[i].p1SourceB];
        temp2 = scale32(temp2, config.channel[i].p1SourceBVolume);
        p1Solution = p1Solution + temp2;
      }
    }

    // Mix in other outputs here (P2)
    if (transition > 0)
    {
      // Mix in dedicated RC sources - aileron, elevator and rudder
      if (config.channel[i].p2AileronVolume != 0)  // Mix in dedicated aileron
      {
        temp2 = scale32(rcInputs[AILERON], config.channel[i].p2AileronVolume);
        p2Solution = p2Solution + temp2;
      }
      if (config.channel[i].p2ElevatorVolume != 0)  // Mix in dedicated elevator
      {
        temp2 = scale32(rcInputs[ELEVATOR], config.channel[i].p2ElevatorVolume);
        p2Solution = p2Solution + temp2;
      }
      if (config.channel[i].p2RudderVolume != 0)  // Mix in dedicated rudder
      {
        temp2 = scale32(rcInputs[RUDDER], config.channel[i].p2RudderVolume);
        p2Solution = p2Solution + temp2;
      }

      // Other sources
      if ((config.channel[i].p2SourceAVolume != 0) && (config.channel[i].p2SourceA != NOMIX))  // Mix in first extra source
      {
        temp2 = universalP2[config.channel[i].p2SourceA];
        temp2 = scale32(temp2, config.channel[i].p2SourceAVolume);
        p2Solution = p2Solution + temp2;
      }
      if ((config.channel[i].p2SourceBVolume != 0) && (config.channel[i].p2SourceB != NOMIX))  // Mix in second extra source
      {
        temp2 = universalP2[config.channel[i].p2SourceB];
        temp2 = scale32(temp2, config.channel[i].p2SourceBVolume);
        p2Solution = p2Solution + temp2;
      }
    }

    // Save solution for this channel. Note that this contains cross-mixed data from the *last* cycle
    config.channel[i].p1Value = p1Solution;
    config.channel[i].p1Value = p1Solution;

  } // Mixer loop: for (i = 0; i < MAX_OUTPUTS; i++)

  //************************************************************
  // Mixer transition code
  //************************************************************

  // Convert number to percentage (0 to 100%)
  if (config.transitionSpeedOut != 0)
  {
    // transition_counter counts from 0 to 100 (101 steps)
    transition = transitionCounter;
  }

  // Recalculate P1 values based on transition stage
  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    // Speed up the easy ones :)
    if (transition == 0)
    {
      temp1 = config.channel[i].p1Value;
    }
    else if (transition >= 100)
    {
      temp1 = config.channel[i].p2Value;
    }
    else
    {
      // Get source channel value
      temp1 = config.channel[i].p1Value;
      temp1 = scale32(temp1, (100 - transition));

      // Get destination channel value
      temp2 = config.channel[i].p2Value;
      temp2 = scale32(temp2, transition);

      // Sum the mixers
      temp1 = temp1 + temp2;
    }
    // Save transitioned solution into P1
    config.channel[i].p1Value = temp1;
  }

  //************************************************************
  // P1/P2 throttle curve handling
  // Work out the resultant Monopolar throttle value based on
  // P1_throttle, P2_throttle and the transition number
  //************************************************************

  // Only process if there is a difference
  if (p1Throttle != p2Throttle)
  {
    // Speed up the easy ones :)
    if (transition == 0)
    {
      e32Temp3 = p1Throttle;
    }
    else if (transition >= 100)
    {
      e32Temp3 = p2Throttle;
    }
    else
    {
      // Calculate step difference in 1/100ths and round
      e32Temp1 = (p2Throttle - p1Throttle);
      e32Temp1 = e32Temp1 << 16;  // Multiply by 65536 so divide gives reasonable step values
      e32Step1 = e32Temp1 / (int32_t)100;

      // Set start (P1) point
      e32Temp2 = p1Throttle;  // Promote to 32 bits
      e32Temp2 = e32Temp2 << 16;

      // Multiply [transition] steps (0 to 100)
      e32Temp3 = e32Temp2 + (e32Step1 * transition);

      // Round, then rescale to normal value
      e32Temp3 = e32Temp3 + (int32_t)32768;
      e32Temp3 = e32Temp3 >> 16;
    }
  }

  // No curve
  else
  {
    // Just use the value of P1_throttle as there is no curve
    e32Temp3 = p1Throttle; // Promote to 16 bits
  }

  // Copy to monopolar throttle
  monoThrottle = (int16_t)e32Temp3;

  //************************************************************
  // Groovy transition curve handling. Must be after the transition.
  // Uses the transition value, but is not part of the transition
  // mixer. Linear or Sine curve. Reverse Sine done automatically
  //************************************************************

  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    // Ignore if both throttle volumes are 0% (no throttle)
    if  (!((config.channel[i].p1ThrottleVolume == 0) &&
           (config.channel[i].p2ThrottleVolume == 0)))
    {
      // Only process if there is a curve
      if (config.channel[i].p1ThrottleVolume != config.channel[i].p2ThrottleVolume)
      {
        // Calculate step difference in 1/100ths and round
        temp1 = (config.channel[i].p2ThrottleVolume - config.channel[i].p1ThrottleVolume);
        temp1 = temp1 << 7;  // Multiply by 128 so divide gives reasonable step values
        step1 = temp1 / 100;

        // Set start (P1) point
        temp2 = config.channel[i].p1ThrottleVolume;  // Promote to 16 bits
        temp2 = temp2 << 7;

        // Linear vs. Sinusoidal calculation
        if (config.channel[i].throttleCurve == LINEAR)
        {
          // Multiply [transition] steps (0 to 100)
          temp3 = temp2 + (step1 * transition);
        }

        // SINE
        else if (config.channel[i].throttleCurve == SINE)
        {
          // Choose between SINE and COSINE
          // If P2 less than P1, COSINE (reverse SINE) is the one we want
          if (step1 < 0)
          {
            // Multiply SIN[100 - transition] steps (0 to 100)
            temp3 = 100 - (int8_t)pgm_read_byte(&sin[100 - (int8_t)transition]);
          }
          // If P2 greater than P1, SINE is the one we want
          else
          {
            // Multiply SIN[transition] steps (0 to 100)
            temp3 = (int8_t)pgm_read_byte(&SIN[(int8_t)transition]);
          }

          // Get SINE% (temp2) of difference in volumes (Step1)
          // Step1 is already in 100ths of the difference * 128
          // temp1 is the start volume * 128
          temp3 = temp2 + (step1 * temp3);
        }
        // SQRT SINE
        else
        {
          // Choose between SQRT SINE and SQRT COSINE
          // If P2 less than P1, COSINE (reverse SINE) is the one we want
          if (step1 < 0)
          {
            // Multiply SQRTSIN[100 - transition] steps (0 to 100)
            temp3 = 100 - (int8_t)pgm_read_byte(&SQRTSIN[100 - (int8_t)transition]);
          }
          // If P2 greater than P1, SINE is the one we want
          else
          {
            // Multiply SQRTSIN[transition] steps (0 to 100)
            temp3 = (int8_t)pgm_read_byte(&SQRTSIN[(int8_t)transition]);
          }

          // Get SINE% (temp2) of difference in volumes (Step1)
          // Step1 is already in 100ths of the difference * 128
          // temp1 is the start volume * 128
          temp3 = temp2 + (step1 * temp3);
        }

        // Round, then rescale to normal value
        temp3 = temp3 + 64;
        temp3 = temp3 >> 7;
      }

      // No curve
      else
      {
        // Just use the value of P1 volume as there is no curve
        temp3 = config.channel[i].p1ThrottleVolume;  // Promote to 16 bits
      }

      // Calculate actual throttle value to the curve
      temp3 = scale32(monoThrottle, temp3);

      // At this point, the throttle values are 0 to 2500 (+/-150%)
      // Re-scale throttle values back to neutral-centered system values (+/-1250)
      // and set the minimum throttle point to 1.1ms.
      // A THROTTLEMIN value of -1000 will result in 2750, or 1.1ms
      temp3 = temp3 - THROTTLEMIN;

      // Add offset to channel value
      config.channel[i].p1Value += temp3;

    } // No throttle

    // No throttles, so clamp to THROTTLEMIN if flagged as a motor
    else if (config.channel[i].motorMarker == MOTORPWM  || config.channel[i].motorMarker == ONESHOT)
    {
      config.channel[i].p1Value = -THROTTLEOFFSET;  // 3750-1250 = 2500 = 1.0ms. THROTTLEOFFSET = 1250
    }
  }

  //************************************************************
  // Per-channel 7-point offset needs to be after the transition
  // loop as it is non-linear, unlike the transition.
  //************************************************************

  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    // The input to the curves will be the transition number, altered to appear as -1000 to 1000.
    temp1 = (transition - 50) * 20; // 0 - 100 -> -1000 to 1000

    // Process as 7-point offset curve. All are BIPOLAR types.
    // Temporarily add NUMBEROFCURVES to the curve number to identify
    // them to Process_curve() as being offsets, not the other curves.
    temp2 = processCurve(i + NUMBEROFCURVES, BIPOLAR, temp1);

    // Add offset to channel value
    config.channel[i].p1Value += temp2;
  }

} // ProcessMixer()

//************************************************************
// Misc mixer code
//************************************************************

// Update actual limits value with that from the mix setting percentages
// This is only done at start-up and whenever the values are changed
// so as to reduce CPU loop load
void updateLimits(void)
{
  uint8_t i, j;
  int32_t temp32, gain32;

  // RPY + Z damp
  int8_t limits[FLIGHT_MODES][NUMBEROFAXIS + 1] =
  {
    {config.flightMode[P1].rollLimit, config.flightMode[P1].pitchLimit, config.flightMode[P1].yawLimit, config.flightMode[P1].aZedLimit},
    {config.flightMode[P2].rollLimit, config.flightMode[P2].pitchLimit, config.flightMode[P2].yawLimit, config.flightMode[P2].aZedLimit}
  };

  // RPY + Z damp
  int8_t gains[FLIGHT_MODES][NUMBEROFAXIS + 1] =
  {
    {config.flightMode[P1].rollIMult, config.flightMode[P1].pitchIMult, config.flightMode[P1].yawIMult, config.flightMode[P1].aZedIMult},
    {config.flightMode[P2].rollIMult, config.flightMode[P2].pitchIMult, config.flightMode[P2].yawIMult, config.flightMode[P2].aZedIMult}
  };

  // Update LVA trigger
  // Vbat is measured in units of 10mV, so powerTriggerActual of 1270 equates to 12.7V
  switch (config.powerTrigger)
  {
    case 0:
      config.powerTriggerActual = 0;    // Off
      break;
    case 1:
      config.powerTriggerActual = 320;  // 3.2V
      break;
    case 2:
      config.powerTriggerActual = 330;  // 3.3V
      break;
    case 3:
      config.powerTriggerActual = 340;  // 3.4V
      break;
    case 4:
      config.powerTriggerActual = 350;  // 3.5V
      break;
    case 5:
      config.powerTriggerActual = 360;  // 3.6V
      break;
    case 6:
      config.powerTriggerActual = 370;  // 3.7V
      break;
    case 7:
      config.powerTriggerActual = 380;  // 3.8V
      break;
    case 8:
      config.powerTriggerActual = 390;  // 3.9V
      break;
    default:
      config.powerTriggerActual = 0;    // Off
      break;
  }

  // Determine cell count and use to multiply trigger
  if (systemVoltage >= 2150)  // 6S - 21.5V or at least 3.58V per cell
  {
    config.powerTriggerActual *= 6;
  }
  else if ((systemVoltage >= 1730) && (systemVoltage < 2150))  // 5S 17.3V to 21.5V or 4.32V(4S) to 3.58V(6S) per cell
  {
    config.powerTriggerActual *= 5;
  }
  else if ((systemVoltage >= 1300) && (systemVoltage < 1730))  // 4S 13.0V to 17.3V or 4.33V(3S) to 3.46V(5S) per cell
  {
    config.powerTriggerActual *= 4;
  }
  else if ((systemVoltage >= 900) && (systemVoltage < 1300))   // 3S 9.0V to 13.0V or 4.5V(2S) to 3.25V(4S) per cell
  {
    config.powerTriggerActual *= 3;
  }
  else if (systemVoltage < 900)                                // 2S Under 9.0V or 3.0V(3S) per cell
  {
    config.powerTriggerActual *= 2;
  }

  // Update I_term input constraints for all profiles
  for (j = 0; j < FLIGHT_MODES; j++)
  {
    // Limits calculation is different for gyros and accs
    for (i = 0; i < (NUMBEROFAXIS); i++)
    {
      temp32  = limits[j][i];  // Promote limit %

      // I-term output (throw). Convert from % to actual count
      // A value of 80,000 results in +/- 1250 or full throw at the output stage
      // This is because the maximum signal value is +/-1250 after division by 64. 1250 * 64 = 80,000
      config.rawILimits[j][i] = temp32 * (int32_t)640;  // 80,000 / 125 = 640

      // I-term source limits. These have to be different due to the I-term gain setting
      // I-term = (gyro * gain) / 32, so the gyro count for a particular gain and limit is
      // Gyro = (I-term * 32) / gain :)
      if (gains[j][i] != 0)
      {
        gain32 = gains[j][i];  // Promote gain value
        config.rawIConstrain[j][i] = (config.rawILimits[j][i] << 5) / gain32;
      }
      else
      {
        config.rawIConstrain[j][i] = 0;
      }
    }

    // Accs
    temp32  = limits[j][ZED];  // Promote limit %

    // I-term output (throw). Convert from % to actual count
    // A value of 80,000 results in +/- 1250 or full throw at the output stage
    // This is because the maximum signal value is +/-1250 after division by 64. 1250 * 64 = 80,000
    config.rawILimits[j][ZED] = temp32 * (int32_t)640;  // 80,000 / 125 = 640

    // I-term source limits. These have to be different due to the I-term gain setting
    // I-term = (gyro * gain) / 4, so the gyro count for a particular gain and limit is
    // Gyro = (I-term * 4) / gain :)
    if (gains[j][ZED] != 0)
    {
      gain32 = gains[j][ZED];  // Promote gain value
      config.rawIConstrain[j][ZED] = (config.rawILimits[j][ZED] << 2) / gain32;
    }
    else
    {
      config.rawIConstrain[j][ZED] = 0;
    }
  }

  // Update travel limits
  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    config.limits[i].minimum = scaleMicros(config.minTravel[i]);
    config.limits[i].maximum = scaleMicros(config.maxTravel[i]);
  }

  // Adjust trim to match 0.01 degree resolution
  // A value of 127 multiplied by 100 = 12700 which in 1/100ths of a degree equates to potentially 127 degrees
  // In reality though, with a more realistic P gain of 10, this equates to potentially 12.7 degrees
  for (i = P1; i <= P2; i++)
  {
    config.rolltrim[i]  = config.flightMode[i].accRollZeroTrim  * 100;
    config.pitchtrim[i] = config.flightMode[i].accPitchZeroTrim * 100;
  }

  // Additional tasks to ensure compatibility with the GUI
  // Move any menu post-processing here so that it happens post-reboot

  // Refresh channel order
  updateChOrder();

  // See if mixer preset has changed. The only time it will ever NOT
  // be "Options" is when the GUI has changed it.
  if (config.preset != OPTIONS)
  {
    loadEEPROMPreset(config.preset);

    // Reset the mixer preset
    config.preset = OPTIONS;
  }

  // Update MPU6050 LPF and reverse sense of menu items
  mpu.setDLPFMode(6 - config.mpu6050LPF);

  // If Armed setting is not ARMED    // HJI 1.7
  if (config.armMode != ARMED)        // HJI 1.7
  {
    generalError |= (1 << DISARMED);  // Set flags to disarmed
    LED_OFF;
  }

  // Work out the P1 orientation from the user's P2 orientation setting
  config.orientationP1 = (int8_t)pgm_read_byte(&p1OrientationLUT[config.orientationP2]);

  saveConfigToEEPROM(); // Save values and return
}

// Update servos from the mixer config.channel[i].p1Value data, add offsets and enforce travel limits
// Input values are -1250 to 1250.  Output values are 2500 to 5000
void updateServos(void)
{
  uint8_t i;
  int16_t temp1 = 0; // Output value

  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    // Servo reverse and trim for the eight physical outputs
    temp1 = config.channel[i].p1Value;

    // Reverse this channel for the eight physical outputs
    if ((i <= MAX_OUTPUTS) && (config.servoReverse[i] == ON))
    { 
      temp1 = -temp1;
    }

    // Add offset value to restore to system compatible value
    temp1 += 3750;

    // Bounds check                  // HJI 1.7
    if (temp1 < 2500) temp1 = 2500;  // HJI 1.7
    if (temp1 > 5000) temp1 = 5000;  // HJI 1.7
    
    // Transfer value to servo
    servoOut[i] = (uint16_t)temp1;   // HJI 1.7
  }
}

// 32 bit multiply/scale for broken GCC
// Returns immediately if multiplier is 100, 0 or -100
int16_t scale32(int16_t value16, int16_t multiplier16)
{
  int32_t temp32 = 0;
  int32_t mult32 = 0;

  // No change if 100% (no scaling)
  if (multiplier16 == 100)
  {
    return value16;
  }

  // Reverse if -100%
  else if (multiplier16 == -100)
  {
    return -value16;
  }

  // Zero if 0%
  else if (multiplier16 == 0)
  {
    return 0;
  }

  // Only do the scaling if necessary
  else
  {
    // GCC is broken bad regarding multiplying 32 bit numbers, hence all this crap...
    mult32 = multiplier16;
    temp32 = value16;
    temp32 = temp32 * mult32;

    // Divide by 100 and round to get scaled value
    temp32 = (temp32 + (int32_t)50) / (int32_t)100; // Constants need to be cast up to 32 bits
    value16 = (int16_t)temp32;
  }

  return value16;
}

// Scale percentages to microsecond (position)
// -125% to 125% is 875 to 2125 us or above and below the ESC cal limits
int16_t scaleMicros(int8_t value)
{
  int16_t temp16;

  // Bounds check                  // HJI 1.7
  if (value < -125) value = -125;  // HJI 1.7
  if (value > 125)  value =  125;  // HJI 1.7

  // 0% = 1000, 100% = 2000
  temp16 = (int16_t)((value * 5) + SERVO_CENTER);  // SERVO_CENTER = 1500

  return temp16;
}

// Scale percentages to internal units (position). Must not be further expanded.
// Input values are -125 to 125. Output values are 2187.5 to 5312.5 (875 to 2125us)
int16_t scalePercent(int8_t value)
{
  int16_t temp16;
  float tempf;

  // 100% = 2500 to 5000
  tempf = (float)value; // Promote
  temp16 = (int16_t)((tempf * 12.5f) + 3750.0f);

  return temp16;
}


// Scale curve percentages to relative position (Bipolar internal units)
// Input values are -100 to 100. Output values are -1000 to 1000.
int16_t scaleThrottleCurvePercentBipolar(int8_t value)
{
  int16_t temp16;
  float   tempF;

  tempF  = (float)value;  // Promote
  temp16 = (int16_t)(tempF * 10.0f);

  return temp16;
}

// Scale curve percentages to relative position (Monopolar internal units)
// Note that a curve percentage is a percentage of the input value.
// Input values are 0 to 100% of full throttle. Output values are 0 to 2000 (full throttle).
int16_t scaleThrottleCurvePercentMono(int8_t value)
{
  int16_t temp16;
  float   tempF;

  tempF  = (float)value;  // Promote
  temp16 = (int16_t)(tempF * 20.0f);

  return temp16;
}

// Process curves. Maximum input values are +/-1000 for Bipolar curves and 0-2000 for monopolar curves.
// Curve number > NUMBEROFCURVES are the offset curves.
// Seven points 0, 17%, 33%, 50%, 67%, 83%, 100%  (Monopolar)
// Seven points -100, 67%, -33%, 0%, 33%, 67%, 100% (Bipolar)
int16_t processCurve(uint8_t curve, uint8_t type, int16_t inputValue)
{
  int16_t outputValue = 0;
  int8_t  zone        = 0;
  int8_t  start       = 0;
  int8_t  end         = 0;
  int16_t startPos    = 0;
  int16_t bracket     = 0;
  int16_t endPos      = 0;
  int32_t temp1       = 0;
  int32_t temp2       = 0;
  int32_t step1       = 0;

  if (type == BIPOLAR)
  {
    // Limit input value to +/-100% (+/-1000)
    if (inputValue < -1000)
    {
      inputValue = -1000;
    }
    if (inputValue > 1000)
    {
      inputValue = 1000;
    }
  }
  else // Monopolar
  {
    // Limit input value to 0 to 100% (0 to 2000)
    if (inputValue < 0)
    {
      inputValue = 0;
    }
    if (inputValue > 2000)
    {
      inputValue = 2000;
    }
  }

  if (type == BIPOLAR)
  {
    // Work out which zone we are in
    if (inputValue < -667)
    {
      zone = 0;
      bracket = -1000;
    }
    else if (inputValue < -333)
    {
      zone = 1;
      bracket = -667;
    }
    else if (inputValue < 0)
    {
      zone = 2;
      bracket = -333;
    }
    else if (inputValue > 667)
    {
      zone = 5;
      bracket = 667;
    }
    else if (inputValue > 333)
    {
      zone = 4;
      bracket = 333;
    }
    else if (inputValue >= 0)
    {
      zone = 3;
      bracket = 0;
    }
  }
  else // Monopolar
  {
    // Work out which zone we are in
    if (inputValue < 333)
    {
      zone = 0;
      bracket = 0;
    }
    else if (inputValue < 667)
    {
      zone = 1;
      bracket = 333;
    }
    else if (inputValue < 1000)
    {
      zone = 2;
      bracket = 667;
    }
    else if (inputValue > 1667)
    {
      zone = 5;
      bracket = 1667;
    }
    else if (inputValue > 1333)
    {
      zone = 4;
      bracket = 1333;
    }
    else if (inputValue >= 1000)
    {
      zone = 3;
      bracket = 1000;
    }
  }

  // Find start/end points of zone
  // Normal curves
  if (curve < NUMBEROFCURVES)
  {
    switch (zone)
    {
      case 0:
        start = config.curve[curve].point1;
        end   = config.curve[curve].point2;
        break;
      case 1:
        start = config.curve[curve].point2;
        end   = config.curve[curve].point3;
        break;
      case 2:
        start = config.curve[curve].point3;
        end   = config.curve[curve].point4;
        break;
      case 3:
        start = config.curve[curve].point4;
        end   = config.curve[curve].point5;
        break;
      case 4:
        start = config.curve[curve].point5;
        end   = config.curve[curve].point6;
        break;
      case 5:
        start = config.curve[curve].point6;
        end   = config.curve[curve].point7;
        break;
      default:
        break;
    }
  }
  // Offsets
  else
  {
    // Correct curve number
    curve = curve - NUMBEROFCURVES;

    switch (zone)
    {
      case 0:
        start = config.offsets[curve].point1;
        end   = config.offsets[curve].point2;
        break;
      case 1:
        start = config.offsets[curve].point2;
        end   = config.offsets[curve].point3;
        break;
      case 2:
        start = config.offsets[curve].point3;
        end   = config.offsets[curve].point4;
        break;
      case 3:
        start = config.offsets[curve].point4;
        end   = config.offsets[curve].point5;
        break;
      case 4:
        start = config.offsets[curve].point5;
        end   = config.offsets[curve].point6;
        break;
      case 5:
        start = config.offsets[curve].point6;
        end   = config.offsets[curve].point7;
        break;
      default:
        break;
    }
  }

  // Work out distance to cover
  // Convert percentages to positions
  if (type == BIPOLAR)
  {
    startPos = scaleThrottleCurvePercentBipolar(start);
    endPos   = scaleThrottleCurvePercentBipolar(end);
  }
  else
  {
    startPos = scaleThrottleCurvePercentMono(start);
    endPos   = scaleThrottleCurvePercentMono(end);
  }

  // Upscale span for best resolution (x 65536)
  temp1 = (int32_t)(endPos - startPos);
  temp1 = temp1 << 16;

  // Divide distance into steps that cover the interval
  step1 = (int32_t)(temp1 / (int32_t)334) ;

  temp2 = startPos;
  temp2 = temp2 << 16;
  temp2 += ((inputValue - bracket) * step1);

  // Reformat into a system-compatible value
  // Divide by 65536
  outputValue = (int16_t)(temp2 >> 16);

  return outputValue;
}
