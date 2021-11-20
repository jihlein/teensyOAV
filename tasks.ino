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

uint8_t  disarmSeconds = 0;

void tasks50Hz(void) 
{ 
  //************************************************************
  //* Measure incoming RC rate and flag no signal
  //************************************************************

  // Check to see if the RC input is overdue (50ms)
  if (rcTimeout > RC_OVERDUE)
  {
    overdue   = true;
    rcTimeout = RC_OVERDUE + 1;
  }

  //************************************************************
  //* Alarms
  //************************************************************

  // If RC signal is overdue, signal RX error message
  if (overdue)
  {
    generalError |= (1 << NO_SIGNAL);   // Set NO_SIGNAL bit
	generalError |= (1 << DISARMED);    // Set flags to disarmed
    LED_OFF;                            // Signal that FC is now disarmed
  }
  // RC signal received normally
    else
  {
    generalError &= ~(1 << NO_SIGNAL);  // Clear NO_SIGNAL bit
  }

  // Turn on buzzer if in alarm state (BUZZER_ON is oscillating)
  if (((generalError & (1 << LVA_ALARM)) ||      // Low battery
       (generalError & (1 << NO_SIGNAL)) ||      // No signal
       (generalError & (1 << THROTTLE_HIGH))) && // Throttle high
       (alarmFlags & (1 << BUZZER_ON))
     )
  {
    // Check buzzer mode first
    if (config.buzzer == ON)
    {
      // HJI LVA = 1;
    }
  }
  else
  {
    // HJI LVA = 0;
  }
  
  //************************************************************
  //* Arm/disarm handling
  //************************************************************

  // All cases - reset arm timer
  if (config.armMode == ARMABLE)
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
          menuMode = IDLE;    // Previously IDLE, which was wrong. 
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
        generalError |= (1 << DISARMED);     // Set flags to disarmed
        LED_OFF;                             // Signal that FC is now disarmed
          
        flightFlags |= (1 << ARM_BLOCKER);  // Block motors for a little while to remove arm glitch
          
        // Force Menu to IDLE immediately unless in vibration test mode
        if (config.vibration == OFF)
        {
          menuMode = IDLE;  
        }

#ifdef ERROR_LOG
add_log(MANUAL);
#endif      

      }

      // Automatic disarm
      // Reset auto-disarm count if any RX activity or set to zero
      if ((flightFlags & (1 << RXACTIVITY)) || (config.disarmTimer == 0))
      {
        disarmTimer = 0;
        disarmSeconds = 0;
      }
    
      // Increment disarm timer (seconds) if armed
      if (disarmTimer > SECOND_TIMER)
      {
        disarmSeconds++;
        disarmTimer = 0;
      }

      // Auto-disarm model if timeout enabled and due
      // Don't allow disarms less than 30 seconds. That's just silly...
      if ((disarmSeconds >= config.disarmTimer) && (config.disarmTimer >= 30))
      {
        // Disarm the FC
        generalError |= (1 << DISARMED);  // Set flags to disarmed
        LED_OFF;                          // Signal that FC is now disarmed

#ifdef ERROR_LOG
add_log(TIMER);
#endif

      }
    }
  } // if (Config.ArmMode == ARMABLE)
    
  // Arm when Armed mode is selected
  else if (config.armMode == ARMED)
  {
    // If disarmed, arm
    if (generalError & (1 << DISARMED))
    {
      generalError &= ~(1 << DISARMED);  // Set flags to armed
    }
      
    LED_ON;
  }
  // Handle cases where RC arming has been selected
  else
  {
    // Check for rising edge of RC input delegated to be the arming input
    if ((rcInputs[config.armChannel] > 500) && (oldArmChannel <= 500))
    {
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
}

void tasks500Hz(void) {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  readGyros();
  readAccels();
  profileTransition();
  imuUpdate(0.002f);
  sensorPID(0.002f);
  
  // Check for throttle reset
  if (monopolarThrottle < THROTTLEIDLE)  // THROTTLEIDLE = 50
  {
    // Reset I-terms at throttle cut. Using memset saves code space
    memset(&integralGyro[P1][ROLL], 0, sizeof(float) * 6);
    integralAccelVertF[P1] = 0.0;
    integralAccelVertF[P2] = 0.0;
  }

  calculatePID();
  processMixer();
  updateServos();
  servoCmds();
}

void menuMethods(void) {
  switch(menuMode) 
  {
    // In IDLE mode, the text "Press for status" is displayed ONCE.
    // If a button is pressed the mode changes to PRESTATUS, where
    // it will wait for the right time to proceed.
    case IDLE:
      if (frame_5Hz) {
        frame_5Hz = false;

        idleScreen();
      }
      
      // If any button is pressed
      if((pinb & 0xF0) != 0xF0)
      {
        menuMode = STATUS;
        // Reset the status screen timeout
        statusSeconds = 0;
      }
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

    // Status screen up, but button still down ;)
    // This is designed to stop the menu appearing instead of the status screen
    // as it will stay in this state until the button is released
    case WAITING_TIMEOUT_BD:
      if (digitalRead(BUTTON1) == 0)
      {
        menuMode = WAITING_TIMEOUT_BD;
        statusSeconds = 0;
      }
      else
      {
        menuMode = WAITING_TIMEOUT;
      }
      break;
                        
    // Status screen up, waiting for timeout or action
    // but button is back up
    case WAITING_TIMEOUT:
      // In status screen, change back to idle after timing out
      // If in vibration test mode, stay in Status
      if ((statusSeconds >= 5000) && (config.vibration == OFF))
      {
        menuMode = IDLE;
      }

      // Jump to menu if button pressed
      else if (digitalRead(BUTTON1) == 0)
      {
        menuMode = MENU;
      }

      // Update status screen five times/sec while waiting to time out
      else if (updateStatusTimer > 200)
      {
        menuMode = STATUS;
      }        
      else
      {
        // Unblock motors if blocked
        flightFlags &= ~(1 << ARM_BLOCKER);
      }     
      break;

    // In MENU mode, 
    case MENU:
      // HJI LVA = 0;  // Make sure buzzer is off :)
      // Disarm the FC
      generalError |= (1 << DISARMED);
      LED_OFF;
      // Start the menu system
      menuMain();
      // Switch back to status screen when leaving menu
      menuMode = STATUS;
      // Reset timeout once back in status screen
      statusSeconds = 0;
      // Reset IMU on return from menu
      resetIMU();        
      break;

    default:
      break;
  }
}
