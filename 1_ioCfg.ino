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

#define ARM_TIMER         1000  // Amount of time the sticks must be held to trigger arm. Currently one second.
#define ARM_TIMER_RESET_1  960  // RC position to reset timer for aileron, elevator and rudder
#define ARM_TIMER_RESET_2   50  // RC position to reset timer for throttle
#define DISARM_TIMER      3000  // Amount of time the sticks must be held to trigger disarm. Currently three seconds.
#define SECOND_TIMER      1000  // Unit of timing for seconds
  
#define LOGLENGTH           20  // Log length for debugging

//***********************************************************
// Enumeration
//***********************************************************

enum availability  {OFF = 0, ON, SCALE, REVERSE, REVERSESCALE};
enum curve         {LINEAR = 0, SINE, SQRTSINE};
enum curves        {P1_THR_CURVE = 0, P2_THR_CURVE, P1_COLL_CURVE, P2_COLL_CURVE, GEN_CURVE_C, GEN_CURVE_D};
enum devices       {ASERVO = 0, DSERVO, MOTOR}; 
enum filters       {HZ5 = 0, HZ10, HZ21, HZ44, HZ94, HZ184, HZ260, NOFILTER};
enum flightFlags   {RXACTIVITY = 0, ARM_BLOCKER};
enum frames        {BASIC = 0, EDIT, ABORT, LOG, CURVE, OFFSET};
enum globalStatus  {IDLE = 0, STATUS, WAITING_TIMEOUT_BD, WAITING_TIMEOUT, MENU};
enum outputs       {OUT1 = 0, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7, OUT8};
enum orientation   {UP_BACK = 0, UP_LEFT, UP_FRONT, UP_RIGHT,
                    BACK_DOWN, BACK_LEFT, BACK_UP, BACK_RIGHT,
                    DOWN_BACK, DOWN_RIGHT, DOWN_FRONT, DOWN_LEFT,
                    FRONT_DOWN, FRONT_RIGHT, FRONT_UP, FRONT_LEFT,
                    LEFT_DOWN, LEFT_FRONT, LEFT_UP, LEFT_BACK,
                    RIGHT_DOWN, RIGHT_BACK, RIGHT_UP, RIGHT_FRONT};
enum polarity      {NORMAL = 0, REVERSED};
enum presets       {QUADX = 0, QUADP, TRICOPTER, BLANK, OPTIONS};
enum profiles      {P1 = 0, P2};
enum psych         {MONOPOLAR = 0, BIPOLAR};
enum rcChannels    {THROTTLE = 0, AILERON, ELEVATOR, RUDDER, GEAR, AUX1, AUX2, AUX3, NOCHAN};
enum reference     {NO_ORIENT = 0, EARTH, MODEL};
enum rpyArrayIndex {ROLL = 0, PITCH, YAW, ZED};
enum rxModes       {SBUS = 0, SPEKTRUM, CPPM_MODE};
enum rxSequ        {JRSEQ = 0, FUTABASEQ, CUSTOM};
enum safety        {ARMABLE = 0, ARMED, SF_GEAR, SF_AUX1, SF_AUX2, SF_AUX3}; 
//                  THROTTLE, CURVE A, CURVE B, COLLECTIVE, THROTTLE, AILERON, ELEVATOR, RUDDER, GEAR, AUX1,  AUX2,  AUX3,  ROLLGYRO, PITCHGYO, YAWGYRO, ACCSMOOTH, PITCHSMOOTH, ROLLACC, PITCHACC, AccZ,  NONE
enum sources       {SRC1,     SRC2,    SRC3,    SRC4,       SRC5,     SRC6,    SRC7,     SRC8,   SRC9, SRC10, SRC11, SRC12, SRC13,    SRC14,    SRC15,   SRC16,     SRC17,       SRC18,   SRC19,    SRC20, NOMIX};
enum transitState  {TRANS_P1 = 0, TRANS_P1_TO_P1N_START, TRANS_P1N_TO_P1_START, TRANS_P1_TO_P2_START, TRANS_P1N, TRANSITIONING, TRANS_P2_TO_P1_START, TRANS_P1N_TO_P2_START, TRANS_P2_TO_P1N_START, TRANS_P2};

//***********************************************************
// Flags
//***********************************************************

enum globalError   {THROTTLE_HIGH = 0, NO_SIGNAL, DISARMED, LVA_ALARM, BUZZER_ON};
enum MainFlags     {INV_CAL_DONE_P1 = 0, NORMAL_CAL_DONE_P1, INV_CAL_DONE_P2, NORMAL_CAL_DONE_P2};
