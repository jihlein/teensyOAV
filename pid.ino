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
// * Notes
// ************************************************************
//
// Servo output range is 2500 to 5000, centered on 3750.
// RC and PID values are added to this then rescaled at the the output stage to 1000 to 2000.
// As such, the maximum usable value that the PID section can output is +/-1250.
// So working backwards, prior to rescaling (/64) the max values are +/-80,000.
// Prior to this, PID_Gyro_I_actual has been divided by 32 so the values are now +/- 2,560,000
// However the I-term gain can be up to 127 which means the values are now limited to +/-20,157 for full scale authority.
// For reference, a constant gyro value of 50 would go full scale in about 1 second at max gain of 127 if incremented at 400Hz.
// This seems about right for heading hold usage.
//
// On the KK2.1 the gyros are configured to read +/-2000 deg/sec at full scale, or 16.4 deg/sec for each LSB value.  
// I divide that by 16 to give 0.976 deg/sec for each digit the gyros show. So "50" is about 48.8 degrees per second.
// 360 deg/sec would give a reading of 368 on the sensor calibration screen. Full stick is about 1000 or so. 
// So with no division of stick value by "Axis rate", full stick would equate to (1000/368 * 360) = 978 deg/sec. 
// With axis rate set to 2, the stick amount is quartered (250) or 244 deg/sec. A value of 3 would result in 122 deg/sec. 
//
// Stick rates: /64 (15.25), /32 (30.5), /16 (61*), /8 (122), /4 (244), /2 (488), /1 (976)

//************************************************************
// Defines
//************************************************************

#define HPF_FC  20
#define HPF_Q 1
#define HPF_O (2 * M_PI * HPF_FC / SAMPLE_RATE)
#define HPF_C (HPF_Q / HPF_O)
#define HPF_L (1 / HPF_Q / HPF_O)
#define PID_SCALE 6  // Empirical amount to reduce the PID values by to make them most useful
#define SAMPLE_RATE 500  

//************************************************************
// Code
//************************************************************

// PID globals for each [Profile] and [axis]
float   gyroSmooth[NUMBEROFAXIS];  // Filtered gyro data
int32_t pidAvgGyro[NUMBEROFAXIS];  // Averaged gyro data over last x loops

float fSample = 0;
float hpfI    = 0;
float hpfT    = 0;
float hpfV    = 0;

// Run each loop to average gyro data and also accVert data
void sensorPID(float period)
{
  float tempF1    = 0;
  float gyroAdcF  = 0;
  int8_t i        = 0;
  int8_t  axis    = 0; 
  int16_t stickP1 = 0;
  int16_t stickP2 = 0;
  int32_t p1Temp  = 0;
  int32_t p2Temp  = 0;
  
  // Cross-reference table for actual RCinput elements
  // Note that axes are reversed here with respect to their gyros
  // So why is AILERON different? Well on the KK hardware the sensors are arranged such that
  // RIGHT roll = +ve gyro, UP pitch = +ve gyro and LEFT yaw = +ve gyro.
  // However the way we have organised stick polarity, RIGHT roll and yaw are +ve, and DOWN elevator is too.
  // When combining with the gyro signals, the sticks have to be in the opposite polarity as the gyros.
  // As described above, pitch and yaw are already opposed, but roll needs to be reversed.

  int16_t rcInputsAxis[NUMBEROFAXIS] = {-rcInputs[AILERON], rcInputs[ELEVATOR], rcInputs[RUDDER]};
  
  int8_t stickRates[FLIGHT_MODES][NUMBEROFAXIS] =
  {
    {config.flightMode[P1].rollRate, config.flightMode[P1].pitchRate, config.flightMode[P1].yawRate},
    {config.flightMode[P2].rollRate, config.flightMode[P2].pitchRate, config.flightMode[P2].yawRate}
  };

  //************************************************************
  // Create a measure of gyro noise
  //************************************************************

  // Only bother when display vibration info is set to "ON"
  if (config.vibration == ON)
  {
    // Work out quick average of all raw gyros and take the absolute value
    fSample = (float)(gyroAdcRaw[ROLL] + gyroAdcRaw[PITCH] + gyroAdcRaw[YAW]);

    // HPF example from http://www.codeproject.com/Tips/681745/Csharp-Discrete-Time-RLC-Low-High-Pass-Filter-Rout
    // Some values preset for a 10Hz cutoff at 500Hz sample rate
    hpfT     = (fSample * HPF_O) - hpfV;
    hpfV    += (hpfI + hpfT) / HPF_C;
    hpfI    += hpfT / HPF_L;
    fSample -= hpfV / HPF_O;

    // LPF filter the readings so that they are more persistent
    gyroAvgNoise = ((gyroAvgNoise * 99.0f) + abs(fSample)) / 100.0f;

    // Limit noise reading to 999
    if (gyroAvgNoise > 999.0f)
    {
      gyroAvgNoise = 999.0f;
    }
  }

  for (axis = 0; axis <= YAW; axis ++)
  {
    //************************************************************
    // Work out stick rate divider. 0 is slowest, 7 is fastest.
    // /64 (15.25), /32 (30.5), /16 (61*), /8 (122), /4 (244), /2 (488), /1 (976), *2 (1952)
    //************************************************************

    if (stickRates[P1][axis] <= 6)
    {
      stickP1 = rcInputsAxis[axis] >> (4 - (stickRates[P1][axis] - 2));
    }
    else
    {
      stickP1 = rcInputsAxis[axis] << ((stickRates[P1][axis]) - 6);
    }
    
    if (stickRates[P2][axis] <= 6)
    {
      stickP2 = rcInputsAxis[axis] >> (4 - (stickRates[P2][axis] - 2));
    }
    else
    {
      stickP2 = rcInputsAxis[axis] << ((stickRates[P2][axis]) - 6);
    }   
    
    //************************************************************
    // Gyro LPF
    //************************************************************  

    // Lookup LPF value
    memcpy_P(&tempF1, &lpfLookup[config.gyroLPF], sizeof(float));
    
    gyroAdcF = gyroAdc[axis]; // Promote gyro signal to suit

    if (config.gyroLPF != NOFILTER)
    {
      // Gyro LPF
      gyroSmooth[axis] = ((gyroSmooth[axis] * (tempF1 - 1.0f)) + gyroAdcF) / tempF1;
    }
    else
    {
      // Use raw gyroADC[axis] as source for gyro values when filter is off
      gyroSmooth[axis] = gyroAdcF;
    }   
    
    // Demote back to int16_t
    gyroAdc[axis] = (int16_t)gyroSmooth[axis];  

    //************************************************************
    // Magically correlate the I-term value with the loop rate.
    // This keeps the I-term and stick input constant over varying 
    // loop rates 
    //************************************************************

    p1Temp = gyroAdc[axis] + stickP1;
    p2Temp = gyroAdc[axis] + stickP2;

    // Adjust gyro and stick values based on period
    // I hate magic numbers, but the 403.14 seems necessary to get
    // the I term to rise to max value in one second with a gyro
    // input of 50, per the initial comments in this file.  Not
    // sure why this is.
    integralGyro[P1][axis] += (float)p1Temp * period * 403.14f;
    integralGyro[P2][axis] += (float)p2Temp * period * 403.14f;
    
    //************************************************************
    // Limit the I-terms to the user-set limits
    //************************************************************
    
    for (i = P1; i <= P2; i++)
    {
      if (integralGyro[i][axis] > (float)config.rawIConstrain[i][axis])
      {
        integralGyro[i][axis] = (float)config.rawIConstrain[i][axis];
      }
      
      if (integralGyro[i][axis] < (float)-config.rawIConstrain[i][axis])
      {
        integralGyro[i][axis] = (float)-config.rawIConstrain[i][axis];
      }
    }

    //************************************************************
    // Sum gyro readings for P-terms for later averaging
    //************************************************************

    pidAvgGyro[axis] += gyroAdc[axis];
  
  } // for (axis = 0; axis <= YAW; axis ++)
    
  //************************************************************
  // Calculate the Z-acc I-term 
  // accVert is already smoothed by AccSmooth, but needs DC 
  // offsets removed to minimize drift.
  // Also, shrink the integral by a small fraction to temper 
  // remaining offsets.
  //************************************************************    
  
  integralAccelVertF[P1] += accelVertF;
  integralAccelVertF[P2] += accelVertF;   

  tempF1 = config.accelVertFilter;  // Promote AccVertfilter (0 to 127)
  tempF1 = tempF1 / 10000.0f;
  tempF1 = 1.0f - tempF1;
  
  integralAccelVertF[P1] = integralAccelVertF[P1] * tempF1;  // Decimator. Shrink integrals by user-set amount
  integralAccelVertF[P2] = integralAccelVertF[P2] * tempF1;

  //************************************************************
  // Limit the Z-acc I-terms to the user-set limits
  //************************************************************
  for (i = P1; i <= P2; i++)
  {
    tempF1 = config.rawIConstrain[i][ZED];  // Promote
    
    if (integralAccelVertF[i] > tempF1)
    {
      integralAccelVertF[i] = tempF1;
    }
      
    if (integralAccelVertF[i] < -tempF1)
    {
      integralAccelVertF[i] = -tempF1;
    }
  }
}

// Run just before PWM output, using averaged data
void calculatePID(void)
{
  int32_t pidGyroTemp1    = 0;  // P1
  int32_t pidGyroTemp2    = 0;  // P2
  int32_t pidAccelTemp1   = 0;  // P
  int32_t pidAccelTemp2   = 0;  // I
  int32_t pidGyroIActual1 = 0;  // Actual unbound i-terms P1
  int32_t pidGyroIActual2 = 0;  // P2
  int32_t p1PGain         = 0;  // HJI 1.7
  int32_t p2PGain         = 0;  // HJI 1.7
  float   p1IGain         = 0;  // HJI 1.7
  float   p2IGain         = 0;  // HJI 1.7
  int8_t  axis = 0;
  int8_t i = 0;

  // Initialise arrays with gain values.
  int8_t  pGain[FLIGHT_MODES][NUMBEROFAXIS] = 
    {
      {config.flightMode[P1].rollPMult, config.flightMode[P1].pitchPMult, config.flightMode[P1].yawPMult},
      {config.flightMode[P2].rollPMult, config.flightMode[P2].pitchPMult, config.flightMode[P2].yawPMult}
    };

  int8_t  iGain[FLIGHT_MODES][NUMBEROFAXIS+1] = 
    {
      {config.flightMode[P1].rollIMult, config.flightMode[P1].pitchIMult, config.flightMode[P1].yawIMult, config.flightMode[P1].aZedIMult},
      {config.flightMode[P2].rollIMult, config.flightMode[P2].pitchIMult, config.flightMode[P2].yawIMult, config.flightMode[P2].aZedIMult}
    };

  int8_t  lGain[FLIGHT_MODES][NUMBEROFAXIS] = 
    {
      {config.flightMode[P1].aRollPMult, config.flightMode[P1].aPitchPMult, config.flightMode[P1].aZedPMult},
      {config.flightMode[P2].aRollPMult, config.flightMode[P2].aPitchPMult, config.flightMode[P2].aZedPMult}
    };

  // Only for roll and pitch acc trim
  int16_t lTrim[FLIGHT_MODES][2] =
    {
      {config.rolltrim[P1], config.pitchtrim[P1]},
      {config.rolltrim[P2], config.pitchtrim[P2]}
    };

  //************************************************************
  // PID loop
  //************************************************************
  
  for (axis = 0; axis <= YAW; axis ++)
  {
    //************************************************************
    // Get average gyro readings for P-terms
    //************************************************************

    gyroAdc[axis] = (int16_t)(pidAvgGyro[axis] / loopCount);
    pidAvgGyro[axis] = 0;  // Reset average

    //************************************************************
    // Add in gyro Yaw trim
    //************************************************************

    if (axis == YAW)
    {
      pidGyroTemp1 = (int32_t)(config.flightMode[P1].yawTrim << 6);
      pidGyroTemp2 = (int32_t)(config.flightMode[P2].yawTrim << 6);
    }
    // Reset PID_gyro variables to that data does not accumulate cross-axis
    else
    {
      pidGyroTemp1 = 0;
      pidGyroTemp2 = 0;
    }

    //************************************************************
    // Calculate PID gains
    //************************************************************

    // Promote gain variables            // HJI 1.7
    p1PGain = (int32_t)pGain[P1][axis];  // HJI 1.7
    p1IGain = (int32_t)iGain[P1][axis];  // HJI 1.7
    p2PGain = (int32_t)pGain[P2][axis];  // HJI 1.7
    p2IGain = (int32_t)iGain[P2][axis];  // HJI 1.7

    // Gyro P-term                                                  // Profile P1
    pidGyroTemp1 += gyroAdc[axis] * p1PGain;                        // Multiply P-term (Max gain of 127)
    pidGyroTemp1  = pidGyroTemp1 * (int32_t)3;                      // Multiply by 3

    // Gyro I-term
    pidGyroIActual1 = (int32_t)(integralGyro[P1][axis] * p1IGain);  // Multiply I-term (Max gain of 127)
    
    pidGyroIActual1 = pidGyroIActual1 >> 5;                         // Divide by 32

    // Gyro P-term                                                  // Profile P2
    pidGyroTemp2 += gyroAdc[axis] * p2PGain;
    pidGyroTemp2  = pidGyroTemp2 * (int32_t)3;

    // Gyro I-term
    pidGyroIActual2 = (int32_t)(integralGyro[P2][axis] * p2IGain);
    pidGyroIActual2 = pidGyroIActual2 >> 5;

    //************************************************************
    // I-term output limits
    //************************************************************

    // P1 limits
    if (pidGyroIActual1 > config.rawILimits[P1][axis]) 
    {
      pidGyroIActual1 = config.rawILimits[P1][axis];
    }
    else if (pidGyroIActual1 < -config.rawILimits[P1][axis]) 
    {
      pidGyroIActual1 = -config.rawILimits[P1][axis];  
    }

    // P2 limits
    if (pidGyroIActual2 > config.rawILimits[P2][axis]) 
    {
      pidGyroIActual2 = config.rawILimits[P2][axis];
    }
    else if (pidGyroIActual2 < -config.rawILimits[P2][axis]) 
    {
      pidGyroIActual2 = -config.rawILimits[P2][axis];  
    }

    //************************************************************
    // Sum Gyro P, I and D terms and rescale
    //************************************************************

    pidGyros[P1][axis] = (int16_t)((pidGyroTemp1 + pidGyroIActual1) >> PID_SCALE);  // Currently PID_SCALE = 6 so /64
    pidGyros[P2][axis] = (int16_t)((pidGyroTemp2 + pidGyroIActual2) >> PID_SCALE);

    //************************************************************
    // Calculate error from angle data and trim (roll and pitch only)
    //************************************************************

    if (axis < YAW)
    {
      // Do for P1 and P2
      for (i = P1; i <= P2; i++)
      {
        pidAccelTemp1  = angle[axis] - lTrim[i][axis];       // Offset angle with trim
        pidAccelTemp1 *= lGain[i][axis];                     // P-term of accelerometer (Max gain of 127)
        pidAccels[i][axis] = (int16_t)(pidAccelTemp1 >> 8);  // Reduce and convert to integer
      }
    }

  } // PID loop (axis)

  //************************************************************
  // Calculate an Acc-Z PI value 
  //************************************************************

  // Do for P1 and P2
  for (i = P1; i <= P2; i++)
  {
    // P-term
    pidAccelTemp1  = (int32_t)-accelVertF;        // Zeroed AccSmooth signal. Negate to oppose G
    pidAccelTemp1 *= lGain[i][YAW];               // Multiply P-term (Max gain of 127)
    pidAccelTemp1  = pidAccelTemp1 * (int32_t)3;  // Multiply by 3

    // I-term
    pidAccelTemp2  = (int32_t)-integralAccelVertF[i];  // Get and copy integrated Z-acc value. Negate to oppose G
    pidAccelTemp2 *= iGain[i][ZED];                    // Multiply I-term (Max gain of 127)
    pidAccelTemp2 = pidAccelTemp2 >> 2;                // Divide by 4

    if (pidAccelTemp2 > config.rawILimits[i][ZED])  // Limit I-term outputs to user-set percentage
    {
      pidAccelTemp2 = config.rawILimits[i][ZED];
    }
    if (pidAccelTemp2 < -config.rawILimits[i][ZED])
    {
      pidAccelTemp2 = -config.rawILimits[i][ZED];
    }

    // Formulate PI value and scale
    pidAccels[i][YAW] = (int16_t)((pidAccelTemp1 + pidAccelTemp2) >> PID_SCALE); // Copy to global values
  }
}
