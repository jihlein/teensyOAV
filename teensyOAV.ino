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
//* Includes
//***********************************************************

#include <DSMRX.h>
#include <MPU6050.h>
#include <myPWMServo.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "accels.h"
#include "gyros.h"
#include "ioCfg.h"
#include "pid.h"
#include "rc.h"
#include "sumdRX.h"

// At the moment, there are some questions/issues revolving around the Arduino sBus
// library versions.  Version 2.1.2, the lastest, runs fine with FrSky sBus, but does
// not work with Futaba sBus.  Futaba sBus, however, does work with an older version,
// version 2.0.0.  No newer versions work with Futaba.  Issue "might" be related to the
// inclusion of SBUS2 headers, but that is as of yet unproven.
//
// So for now, uncomment the 2p1p2 include line when using FrSky, and comment  the line
// for the 2p0p0 include.  Do the opposite when using Futaba.  Not a good long term
// solution, but....

// Uncomment for FrSky sBus, comment for Futaba (Hopefully temporary)
#define FRSKYSBUS

#if defined FRSKYSBUS
  #include <sbus.h>
  bfs::SbusRx sbusRx(&Serial3);
  bfs::SbusData data;
#else
  #include <sbus2p0p0.h>
  SbusRx sbusRx(&Serial3);
#endif

DSM1024 dsm;

SumdRx *sumdDecoder;
bool   sumdFailSafe = true;

// Uncomment only one of the following OLED dislay drivers:
// SPI Drivers:
// U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

MPU6050 mpu;

myPWMServo output0;
myPWMServo output1;
myPWMServo output2;
myPWMServo output3;
myPWMServo output4;
myPWMServo output5;
myPWMServo output6;
myPWMServo output7;
myPWMServo output8;
myPWMServo output9;

//***********************************************************
//* Defines
//***********************************************************

#define ARM_TIMER          1000  // Amount of time the sticks must be held to trigger arm. Currently one second.
#define ARM_TIMER_RESET_1  960   // RC position to reset timer for aileron, elevator and rudder
#define ARM_TIMER_RESET_2  50    // RC position to reset timer for throttle
#define DISARM_TIMER       3000  // Amount of time the sticks must be held to trigger disarm. Currently three seconds.
#define RC_OVERDUE         5000  // Number of 1 mSec ticks before RC will be overdue = 5 Sec
#define SECOND_TIMER       1000  // Unit of timing for seconds

//***********************************************************
//* Code and Data variables
//***********************************************************

// Flight variables
int16_t oldTransition = 0;
uint8_t oldTransitionState = TRANS_P1;
int16_t transition = 0;
int16_t transitionCounter = 0;
uint8_t transitionState = TRANS_P1;

// Flags
volatile uint8_t alarmFlags   = 0;
volatile uint8_t flightFlags  = 0;
volatile uint8_t generalError = 0;
volatile bool    transitionUpdated = false;

// Global Buffers
#define PBUFFER_SIZE 25
char pBuffer[PBUFFER_SIZE];      // Print buffer (25 bytes)

// Locals
uint8_t  disarmSeconds = 0;
uint8_t  menuMode      = STATUS_TIMEOUT;
uint8_t  oldAlarms     = 0;
uint16_t oldArmChannel = 0;
int8_t   oldFlight = 3;     // Old flight profile
int8_t   oldTransMode = 0;  // Old transition mode
uint8_t  statusSeconds = 0;
uint8_t  transitionDirection = P2;
uint16_t transitionTime = 0;

// Transition matrix
// Usage: Transition_state = Trans_Matrix[config.FlightSel][old_flight]
// config.FlightSel is where you've been asked to go, and old_flight is where you were.
// Transition_state is where you end up :)
const int8_t transMatrix[3][3] PROGMEM =
{
  {TRANSITIONING,         TRANS_P1N_TO_P1_START, TRANS_P2_TO_P1_START},
  {TRANS_P1_TO_P1N_START, TRANSITIONING,         TRANS_P2_TO_P1N_START},
  {TRANS_P1_TO_P2_START,  TRANS_P1N_TO_P2_START, TRANSITIONING}
};

// Misc Globals
volatile uint8_t loopCount;
volatile bool    overdue = true;
volatile uint8_t pinb    = 0xFF;

// Timing Variables
uint16_t armTimer          = 0;
uint16_t disarmTimer       = 0;
uint16_t gyroTimeout       = 0;
uint16_t rcTimeout         = RC_OVERDUE;
uint16_t statusTimeout     = SECOND_TIMER;
uint16_t systemTick        = 1;
uint16_t transitionTimeout = 0;
uint16_t updateStatusTimer = 0;

IntervalTimer execTimer;

bool run500Hz;
bool run250Hz;
bool run50Hz;
bool run25Hz;

uint8_t heartbeatLedState = LOW;

//************************************************************
//* Setup
//************************************************************

void setup() {
  Serial.begin(115200);
  
  // Call Init Functions
  init();
  
  // Setup Exec ISR
  execTimer.begin(execCount, 1000);  // run every mSec
}

//************************************************************
//* Exec function
//************************************************************

void execCount(void) {
  armTimer++;
  disarmTimer++;
  gyroTimeout++;
  rcTimeout++;
  statusTimeout++;
  systemTick++;
  transitionTimeout++;
  updateStatusTimer++;
  
  if (systemTick > 1000)
    systemTick = 1;

  if ((systemTick % 2) == 0)
    run500Hz = true;

  if ((systemTick % 4) == 0)
    run250Hz = true;

  if ((systemTick % 20) == 0)
    run50Hz = true;

  if ((systemTick % 40) == 0)
    run25Hz = true;

  pinb = digitalRead(BUTTON1) << 7 |
         digitalRead(BUTTON2) << 6 |
         digitalRead(BUTTON3) << 5 |
         digitalRead(BUTTON4) << 4;
}

//************************************************************
//* Main loop
//************************************************************

void loop() 
{
  // Increment the loop counter
  loopCount++;
    
  //************************************************************
  //* Once per second events
  //* - Increment Status_seconds
  //* - Blink Heartbeat LED
  //* - Check the battery voltage
  //************************************************************

  // Count elapsed seconds
  if (statusTimeout > SECOND_TIMER)
  {
    statusSeconds++;
    statusTimeout = 0;
  
    // Blink Heartbeat LED
    if (heartbeatLedState == LOW)
      heartbeatLedState = HIGH;
    else
      heartbeatLedState = LOW;
  
    digitalWrite(HEARTBEAT_LED, heartbeatLedState);
      
    // Check if Vbat lower that trigger
    if (0)  // HJI Complete this
      generalError |= (1 << LVA_ALARM);   // Set LVA_Alarm flag
    else
      generalError &= ~(1 << LVA_ALARM);  // Clear LVA_Alarm flag
  }

  //************************************************************
  //* State machine for switching between screens safely
  //* In the state machine, once a state changes, the new state 
  //* will be processed in the next loop.
  //************************************************************

  switch (menuMode)
  {
    // In IDLE mode, the text "Press for status" is displayed ONCE.
    // If a button is pressed the mode changes to PRESTATUS, where
    // it will wait for the right time to proceed.
    case IDLE:
      // If any button is pressed
      if ((pinb & 0xF0) != 0xF0)
      {
        menuMode = PRESTATUS;
        // Reset the status screen timeout
        statusSeconds = 0;
      }
      break;

    // Waiting to safely enter Status screen
    case PRESTATUS:
      // Ready to move on
      menuMode = STATUS;
      break;

    // Status screen first display
    case STATUS:
      // Reset the status screen period
      updateStatusTimer = 0;

      // Update status screen
      displayStatus();

      // Wait for timeout
      menuMode = WAITING_TIMEOUT_BD;
      break;

    // Status screen up, but button still down
    // This is designed to stop the menu appearing instead of the status screen
    // as it will stay in this state until the button is released
    case WAITING_TIMEOUT_BD:
      if (digitalRead(BUTTON1) == 0)
        menuMode = WAITING_TIMEOUT_BD;
      else
        menuMode = WAITING_TIMEOUT;
      break;

    // Status screen up, waiting for timeout or action
    // but button is back up
    case WAITING_TIMEOUT:
      // In status screen, change back to idle after timing out
      // If in vibration test mode, stay in Status
      if ((statusSeconds >= 10) && (config.vibration == OFF))
        menuMode = PRESTATUS_TIMEOUT;
      // Jump to menu if button pressed
      else if (digitalRead(BUTTON1) == 0)
        menuMode = MENU;
      // Update status screen four times/sec while waiting to time out
      else if (updateStatusTimer > (SECOND_TIMER >> 2))
        menuMode = PRESTATUS;
      else
        // Unblock motors if blocked
        flightFlags &= ~(1 << ARM_BLOCKER);
      break;

    // Attempting to leave Status gracefully
    case PRESTATUS_TIMEOUT:
      menuMode = STATUS_TIMEOUT;
      break;

    // In STATUS_TIMEOUT mode, the idle screen is displayed and the mode 
    // changed to POSTSTATUS_TIMEOUT. 
    case STATUS_TIMEOUT:
      // Pop up the Idle screen
      idleScreen();

      // Switch to IDLE mode
      menuMode = POSTSTATUS_TIMEOUT;
      break;

    // In POSTSTATUS_TIMEOUT mode, we wait for a PWM cycle to complete
    // The idle screen has been refreshed and we need to wait.
    case POSTSTATUS_TIMEOUT:
      // Switch to IDLE mode
      menuMode = IDLE;
      break;

    // In MENU mode
    case MENU:
      // HJI LVA = 0;  // Make sure buzzer id off
      // Disarm the FC
      generalError |= (1 << DISARMED);
      LED_OFF;
      // Start the menu system
      menuMain();
      // Switch back to status screen when leaving menu
      menuMode = PRESTATUS;
      // Reset timeout once back in status screen
      statusSeconds = 0;
      // Reset IMU on return from menu
      resetIMU();
      break;

    default:
      break;
  }

  //************************************************************
  //* Alarms
  //************************************************************

  // If RC signal is overdue, signal RX error message
  if (overdue)
  {
    generalError |= (1 << NO_SIGNAL);   // Set NO_SIGNAL bit
    generalError |= (1 << DISARMED);    // Set DISARMED bit
    LED_OFF;
  }
  else
    // RC signal received normally
    generalError &= ~(1 << NO_SIGNAL);  // Clear NO_SIGNAL bit

  // Turn on buzzer if in alarm state (BUZZER_ON is oscillating)
  if (((generalError & (1 << LVA_ALARM)) ||       // Low battery
       (generalError & (1 << NO_SIGNAL)) ||       // No signal
       (generalError & (1 << THROTTLE_HIGH))) &&  // Throttle high
       (alarmFlags & (1 << BUZZER_ON)))
  {
    // Check buzzer mode first
    // HJI if (config.buzzer == ON)
      // HJI LVA = 1;
    // HJI else 
      // HJI LVA = 0;
  }

  //************************************************************
  //* Arm/disarm handling
  //************************************************************

  // Arm when Armed mode is selected
  if (config.armMode == ARMED) 
  {
    // If disarmed, arm
    if (generalError & (1 << DISARMED))
      generalError &= ~(1 << DISARMED);  // Set flags to armed
      
    LED_ON;
  }
  else if (config.armMode == ARMABLE)
  {
    // Manual arm/disarm
    // If sticks not at extremes, reset manual arm/disarm timer
    // Sticks down and inside = armed. Down and outside = disarmed
    if (((-ARM_TIMER_RESET_1 < rcInputs[AILERON])  && (rcInputs[AILERON]  < ARM_TIMER_RESET_1)) ||
        ((-ARM_TIMER_RESET_1 < rcInputs[ELEVATOR]) && (rcInputs[ELEVATOR] < ARM_TIMER_RESET_1)) ||
        ((-ARM_TIMER_RESET_1 < rcInputs[RUDDER])   && (rcInputs[RUDDER]   < ARM_TIMER_RESET_1)) ||
         (ARM_TIMER_RESET_2  < monopolarThrottle))
    {
      armTimer = 0;
    }
  
    // If disarmed, arm if sticks held
    if (generalError & (1 << DISARMED))
    {
      // Reset auto-disarm count
      disarmTimer = 0;
      disarmSeconds = 0;
                
      // If arm timer times out, the sticks must have been at extremes for ARM_TIMER seconds
      // If aileron is at min, arm the FC
      if ((armTimer > ARM_TIMER) && (rcInputs[AILERON] < -ARM_TIMER_RESET_1))
      {
        armTimer = 0;
        generalError &= ~(1 << DISARMED);   // Set flags to armed (negate disarmed)
        calibrateGyrosSlow();               // Calibrate gyros (also saves to eeprom)
        LED_ON;                             // Signal that FC is ready

        flightFlags |= (1 << ARM_BLOCKER);  // Block motors for a little while to remove arm glitch
        
        // Force Menu to IDLE immediately unless in vibration test mode
        if (config.vibration == OFF)
        {
          menuMode = PRESTATUS_TIMEOUT;     // Previously IDLE, which was wrong. 
        }
      }
    } 
    // If armed, disarm if sticks held
    else 
    {
      // Disarm the FC after DISARM_TIMER seconds if aileron at max
      if ((armTimer > DISARM_TIMER) && (rcInputs[AILERON] > ARM_TIMER_RESET_1))
      {
        armTimer = 0;
        generalError |= (1 << DISARMED);    // Set flags to disarmed
        LED_OFF;                            // Signal that FC is now disarmed
          
        flightFlags |= (1 << ARM_BLOCKER);  // Block motors for a little while to remove arm glitch
          
        // Force Menu to IDLE immediately unless in vibration test mode
        if (config.vibration == OFF)
        {
          menuMode = PRESTATUS_TIMEOUT;  
        }
      }
    }
  } // else if (Config.ArmMode == ARMABLE)
  // Handle cases where RC arming has been selected
  else
  {
    // Check for rising edge of RC input delegated to be the arming input
    if ((rcInputs[config.armChannel] > 500) && (oldArmChannel <= 500))
    {
      disarmTimer = 0;
      disarmSeconds = 0;

      // If disarmed and throttle below minimum, arm
      if ((generalError & (1 << DISARMED)) && (monopolarThrottle < THROTTLEIDLE))
      {
        generalError &= ~(1 << DISARMED);     // Set flags to armed
        LED_ON;                               // Signal the FC is now armed
      }
      // If armed, disarm
      else if (~(generalError & (1 << DISARMED)))
      {
        generalError |= (1 << DISARMED);      // Set flags to disarmed
        LED_OFF;                              // Signal that FC is now disarmed
      }
    }
    oldArmChannel = rcInputs[config.armChannel];
  }

  if ((config.armMode != ARMED) && ((generalError & (1 << DISARMED)) == 0)) 
  {
    // Automatic disarm
    // Reset auto-disarm count if any RX activity or set to zero
    if ((flightFlags & (1 << RXACTIVITY)) || (config.disarmTimer == 0)) {
      disarmTimer = 0;
      disarmSeconds = 0;
    }
    
    // Increment disarm timer (seconds) if armed
    if (disarmTimer > SECOND_TIMER) {
      disarmSeconds++;
      disarmTimer = 0;
    }

    // Auto-disarm model if timeout enabled and due
    // Don't allow disarms less than 30 seconds. That's just silly...
    if ((disarmSeconds >= config.disarmTimer) && (config.disarmTimer >= 30)) {
      // Disarm the FC
      generalError |= (1 << DISARMED);  // Set flags to disarmed
      LED_OFF;                          // Signal that FC is now disarmed
      disarmTimer = 0;
      disarmSeconds = 0;
    } 
  }

  //************************************************************
  //* Get RC data
  //************************************************************

  // Update zeroed RC channel data
  rxGetChannels();

  //************************************************************
  //* Transition between flight profiles
  //************************************************************
 
  fpTransition();
   
  //************************************************************
  //* Measure incoming RC rate and flag no signal
  //************************************************************

  // Check to see if the RC input is overdue (50ms)
  if (rcTimeout > RC_OVERDUE)
    overdue = true;  // This results in a "No Signal" error

  //***********************************************************
  //* Read sensors
  //************************************************************

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  readGyros();
  readAccels();

  //************************************************************
  //* Update attitude, average acc values each loop, and
  //* Update I-terms, average gyro values each loop
  //************************************************************
  if (run500Hz) {
    run500Hz = false;
  
    imuUpdate(0.002);
    sensorPID(0.002);
    calculatePID();  // Calculate PID values
    processMixer();  // Do all the mixer tasks - can be very slow
    updateServos();  // Transfer Config.Channel[i].value data to 
                     // ServoOut[i] and check servo limits. 
                     // Note that values are now at system levels
                     // (were centered around zero, now centered around 3750)
    servoCmds();

    send500HzServos();

    loopCount = 0;
  }

  if (run250Hz) {
    run250Hz = false;
    send250HzServos();
  }
  
  if (run50Hz) {
    run50Hz = false;
    send50HzServos();
  }

  // Check for throttle reset
  if (monopolarThrottle < THROTTLEIDLE)  // THROTTLEIDLE = 50
  {
    // Clear throttle high error
    generalError &= ~(1 << THROTTLE_HIGH);

    // Reset I-terms at throttle cut. Using memset saves code space
    memset(&integralGyro[P1][ROLL], 0, sizeof(int32_t) * 6);
    integralAccelVertF[P1] = 0.0;
    integralAccelVertF[P2] = 0.0;
  }

  //************************************************************
  //* Carefully update idle screen if error level changed
  //************************************************************

  // Only update idle when error state has changed.
  // This prevents the continual updating of the LCD disrupting the FC
  if ((oldAlarms != generalError) && (menuMode == IDLE))
    // Force safe update of idle screen
    menuMode = PRESTATUS_TIMEOUT;

  // Save current alarm state into old_alarms
  oldAlarms = generalError;
}
