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

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

// **************************************************************************
//  * Defines
// **************************************************************************

#define MAX_RC_CHANNELS 8   // Maximum input channels from RX
#define MAX_OUTPUTS 10      // Maximum output channels
#define FLIGHT_MODES 2      // Number of flight profiles
#define NUMBEROFAXIS 3      // Number of axis (Roll, Pitch, Yaw)
#define NUMBEROFCURVES 6    // Number of curves available
#define NUMBEROFPOINTS 7    // Number of points on a curve
#define NUMBEROFORIENTS 24  // Number board orientations
#define NUMBEROFSOURCES 21  // Number of universal input sources

#define MOTOR_0           1100  // PWM value to produce a 1.1 mSec throttle pulse regardless of pulse width mode
#define MOTOR_100         1900  // PWM value to produce a 1.9 mSec throttle pulse regardless of pulse width mode
#define MOTORMIN          1000  // PWM value for throttle cut. 1000 or 1.0 mSec regardless of pulse width mode
#define SERVO_CENTER      1500  // Servo center position. 1500 uSec
#define THROTTLEIDLE        50  // Throttle value below which is considered idle
#define THROTTLEMIN       1000  // Minimum throttle input offset value. 3750-1000 = 2750 or 1.1 mSec
                                // Not to be confused with MOTORMIN which is a PWM value.
#define THROTTLEOFFSET    1250  // Mixer offset needed to reduce the output center to MOTORMIN


#define LOGLENGTH           20  // Log length for debugging

// **************************************************************************
// * Type definitions
// **************************************************************************

// Curves (7)
typedef struct
{
  int8_t    point1;   // First point in the curve
  int8_t    point2;
  int8_t    point3;
  int8_t    point4;
  int8_t    point5;
  int8_t    point6; 
  int8_t    point7;   // Last point in the curve
  int8_t    channel;  // Associated channel
} curve_t;

// Servo limits (4)
typedef struct
{
  int16_t   minimum;
  int16_t   maximum;
} servoLimits_t;

// Flight_control type (20)
typedef struct
{
  int8_t    rollPMult;         // Roll PI
  int8_t    rollIMult;
  int8_t    rollLimit;         // I-term limits (0 to 125%)
  int8_t    rollRate;          // 0 to 4, 1 (Default)
  int8_t    aRollPMult;        // Acc gain settings
  int8_t    accRollZeroTrim;   // User-set ACC trim (+/-127)
  int8_t    pitchPMult;        // Pitch PI
  int8_t    pitchIMult;
  int8_t    pitchLimit;        // I-term limits (0 to 125%)
  int8_t    pitchRate;         // 0 to 4, 1 (Default)
  int8_t    aPitchPMult;
  int8_t    accPitchZeroTrim;
  int8_t    yawPMult;          // Yaw PI
  int8_t    yawIMult;
  int8_t    yawLimit;          // I-term limits (0 to 125%)
  int8_t    yawRate;           // 0 to 4, 1 (Default)
  int8_t    yawTrim;
  int8_t    aZedPMult;         // Vertical velocity damping
  int8_t    aZedIMult;
  int8_t    aZedLimit;
} flightControl_t;

// Channel mixer definition 34 bytes
typedef struct
{
  int16_t   p1Value;           // Current value of this channel at P1
  int16_t   p2Value;           // Current value of this channel at P2
  
  // Mixer menu (30 bytes, 30 items)
  int8_t    motorMarker;       // Motor/Servo marker

  int8_t    p1ThrottleVolume;  // Percentage of throttle to use in P1
  int8_t    p2ThrottleVolume;  // Percentage of throttle to use in P2
  int8_t    throttleCurve;     // Throttle transition curve (Linear, Sine)

  int8_t    p1AileronVolume;   // Percentage of aileron to use in P1
  int8_t    p2AileronVolume;   // Percentage of aileron to use in P2
  int8_t    p1ElevatorVolume;  // Percentage of elevator to use in P1
  int8_t    p2ElevatorVolume;  // Percentage of elevator to use in P2
  int8_t    p1RudderVolume;    // Percentage of rudder to use in P1
  int8_t    p2RudderVolume;    // Percentage of rudder to use in P2

  int8_t    p1RollGyro;        // P1 roll_gyro (OFF/ON/REV/SCALED/REVSCALED)
  int8_t    p2RollGyro;        // P2 roll_gyro
  int8_t    p1PitchGyro;       // P1 pitch_gyro
  int8_t    p2PitchGyro;       // P2 pitch_gyro
  int8_t    p1YawGyro;         // P1 yaw_gyro
  int8_t    p2YawGyro;         // P2 yaw_gyro
  int8_t    p1RollAccel;       // P1 roll_accel
  int8_t    p2RollAccel;       // P2 roll_accel
  int8_t    p1PitchAccel;      // P1 pitch_accel
  int8_t    p2PitchAccel;      // P2 pitch_accel
  int8_t    p1ZDeltaAccel;     // P1 Z_delta_accel
  int8_t    p2ZDeltaAccel;     // P2 Z_delta_accel

  int8_t    p1SourceA;         // Source A for calculation
  int8_t    p1SourceAVolume;   // Percentage of source to use
  int8_t    p2SourceA;         // Source A for calculation
  int8_t    p2SourceAVolume;   // Percentage of source to use
  int8_t    p1SourceB;         // Source B for calculation
  int8_t    p1SourceBVolume;   // Percentage of source to use
  int8_t    p2SourceB;         // Source B for calculation
  int8_t    p2SourceBVolume;   // Percentage of source to use

} channel_t;

// Config settings structure (size)[offset]
typedef struct
{
  // Signature (1)[0]
  uint8_t   setup;          // Byte to identify if already setup

  // Menu adjustable items
  // RC settings (8)[1]
  int8_t    channelOrder[MAX_RC_CHANNELS];  // Assign channel numbers to hard-coded channel order
                                            // OpenAeroVtol uses Thr, Ail, Ele, Rud, Gear, Aux1, Aux2, Aux3
                                            // THROTTLE will always return the correct data for the assigned throttle channel
                                            // AILERON will always return the correct data for the assigned aileron channel
                                            // ELEVATOR will always return the correct data for the assigned elevator channel
                                            // RUDDER will always return the correct data for the assigned rudder channel
  // Servo travel limits (32)[9]
  servoLimits_t  limits[MAX_OUTPUTS];       // Actual, respanned travel limits to save recalculation each loop

  // RC items (12)[41]
  int8_t    rxMode;                         // PWM, CPPM or serial types
  int8_t    txSeq;                          // Channel order of transmitter (JR/Futaba etc)
  int8_t    flightChan;                     // Channel number to select flight mode
  int8_t    transitionSpeedOut;             // Outbound transition speed/channel 0 = tied to channel, 1 to 40 seconds.
  int8_t    transitionSpeedIn;              // Inbound transition  speed/channel 0 = tied to channel, 1 to 40 seconds.
  int8_t    transitionP1;                   // Transition point as a percentage 0% to 99%
  int8_t    transitionP1n;                  // Transition point as a percentage 1% to 99%
  int8_t    transitionP2;                   // Transition point as a percentage 1% to 100%
  int8_t    vibration;                      // Vibration test mode on/off
  int8_t    accelVertFilter;                // Acc Z filter for I-terms in 1/100%
  
  // Flight mode settings (40)[53]
  flightControl_t flightMode[FLIGHT_MODES];  // Flight control settings

  // Servo travel limits (32)[93]
  int32_t   rawILimits[FLIGHT_MODES][NUMBEROFAXIS+1];     // Actual, unspanned I-term output limits to save recalculation each loop (RPY + Z) 4 x 2 x 4 = 32
  
  // Servo constraints (32)[125]
  int32_t   rawIConstrain[FLIGHT_MODES][NUMBEROFAXIS+1];  // Actual, unspanned I-term input limits to save recalculation each loop (RPY + Z) 4 x 2 x 4 = 32

  // Triggers (2)[157]
  uint16_t  powerTriggerActual;             // LVA alarm * 10;

  // General items (12)[159]
  int8_t    orientationP2;                  // P2 orientation
  int8_t    p1Reference;                    // Hover plane of reference (NO, EARTH, VERT_AP)
  int8_t    armMode;                        // Arming mode on/off
  int8_t    disarmTimer;                    // Auto-disarm setting
  int8_t    powerTrigger;                   // LVA cell voltage (0 to 5 for  = 3.5V to 3.9V)
  int8_t    mpu6050LPF;                     // MPU6050's internal LPF. Values are 0x06 = 5Hz, (5)10Hz, (4)21Hz, (3)44Hz, (2)94Hz, (1)184Hz LPF, (0)260Hz
  int8_t    accLPF;                         // LPF for accelerometers
  int8_t    gyroLPF;                        // LPF for gyros
  int8_t    cfFactor;                       // Autolevel correction rate
  int8_t    preset;                         // Mixer preset
  int8_t    buzzer;                         // Buzzer control ON/OFF
  
  // Channel configuration (340)[171] #$(272)[171]
  channel_t channel[MAX_OUTPUTS];           // Channel mixing data  

  // Servo menu (30)[511] #$(24)[443]
  int8_t    servoReverse[MAX_OUTPUTS];      // Reversal of output channel
  int8_t    minTravel[MAX_OUTPUTS];         // Minimum output value (-125 to 125)
  int8_t    maxTravel[MAX_OUTPUTS];         // Maximum output value (-125 to 125)

  // RC inputs (16)[541] #$(16)[467]
  int16_t   rxChannelZeroOffset[MAX_RC_CHANNELS];  // RC channel offsets for actual radio channels

  // P1 Acc zeros (12)[557] #$(12)[483]
  int16_t   accelZeroP1[NUMBEROFAXIS];      // P1 Accel calibration results. Note: Accel-Z zero centered on 1G (about +124)
  int16_t   accelZeroNormZP1;               // Accel-Z zero for normal Z values
  int16_t   accelZeroInvZP1;                // Accel-Z zero for inverted Z values
  int16_t   accelZeroDiffP1;                // Difference between normal and inverted Accel-Z zeros

  // Gyro zeros (6)[569] #$(6)[495]
  int16_t   gyroZeroP1[NUMBEROFAXIS];       // NB. These are now for P1 only

  // Airspeed zero (2)[575] #$(2)[501]
  int16_t   airspeedZero;                   // Zero airspeed sensor offset

  // Flight mode (1)[577] #$(1)[503]
  int8_t    flightSel;                      // User set flight mode

  // Adjusted trims (8)[578] #$(8)[504]
  int16_t   rolltrim[FLIGHT_MODES];         // User set trims * 100
  int16_t   pitchtrim[FLIGHT_MODES];

  // Sticky flags (1)[586] #$(1)[512]
  uint8_t   mainFlags;                      // Non-volatile flags

  // Misc (2)[587] #$(2)[513]
  int8_t    rudderPol;                      // Rudder RC input polarity (V1.1 stops here...)
  int8_t    aileronPol;                     // Aileron RC input polarity
    
  // Error log (21)[589] #$(21)[515]
  int8_t    logPointer;
  int8_t    log[LOGLENGTH];
  
  // P2 Acc zeros (12)[610] #$(12)[536]
  int16_t   accelZeroP2[NUMBEROFAXIS];      // P2 Accel calibration results. Note: Accel-Z zero centered on 1G (about +124)
  int16_t   accelZeroNormZP2;               // Accel-Z zero for normal Z values
  int16_t   accelZeroInvZP2;                // Accel-Z zero for inverted Z values
  int16_t   accelZeroDiffP2;                // Difference between normal and inverted Accel-Z zeros
  
  // P2 Gyro zeros (6)[622] #4(6)[548]
  int16_t   gyroZeroP2[NUMBEROFAXIS];       // NB. These are for P2 only

  // Advanced items (1)[628] #$(1) [554]
  int8_t    orientationP1;                  // P1 orientation
  
  // Curves (48)[629] #$(48) [555]
  curve_t   curve[NUMBEROFCURVES];
  
  // Custom channel order (8)[677] #$(8) [603]
  int8_t    customChannelOrder[MAX_RC_CHANNELS];
  
  // Output offsets (64)[685] #$(64) [611]
  curve_t   offsets[MAX_OUTPUTS];
  
  // Misc (2)[749] #$(1)[675]
  int8_t    elevatorPol;                    // Elevator RC input polarity
  int8_t    armChannel;                     // RC-based arming channel selection

  // [751]

} CONFIG_STRUCT;

// Misc structures
typedef struct
{
  int8_t  lower;         // Lower limit for menu item
  int8_t  upper;         // Upper limit for menu item
  uint8_t increment;     // Increment for menu item
  uint8_t style;         // 0 = numeral, 1 = text
  int8_t  defaultValue;  // Default value for this item
} menuRange_t; 

#endif