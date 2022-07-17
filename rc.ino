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

#define NOISE_THRESH 50  // Max RX noise threshold

// ************************************************************
// * Variables
// ************************************************************

//************************************************************
// Code
//************************************************************

void rxGetChannels(void)
{
  static int16_t oldRxSum;
  int16_t rxSum;
  int16_t rxSumDiff;
    
  if (config.rxMode == SBUS)
  {
    if (sbusRx.Read())
    {
      for (uint8_t i = 0; i < MAX_RC_CHANNELS; i++) 
      {
#if defined FRSKYSBUS
        rawChannels[i] = (sbusRx.ch()[config.channelOrder[i]] - 1024) * 1.22f;
#else
        rawChannels[i] = (sbusRx.rx_channels()[config.channelOrder[i]] - 1024) * 1.22f;
#endif
        rcInputs[i] = rawChannels[i] - config.rxChannelZeroOffset[i];
      }      

      if (!sbusRx.failsafe() && !sbusRx.lost_frame())
      {
        rcTimeout = 0;
        overdue = false;
      }
    }
  }
  else if (config.rxMode == SPEKTRUM)
  {
    uint16_t values[MAX_RC_CHANNELS - 1];
    
    if (dsm.timedOut(micros()))
    {
      // Do nothing
    }
    else if (dsm.gotNewFrame())
    {
      dsm.getChannelValues(values, MAX_RC_CHANNELS - 1);
      
      for (uint8_t i = 0; i < MAX_RC_CHANNELS - 1; i++) 
      {
        rawChannels[i] = (values[config.channelOrder[i]] - 1500) * 3.14f;
        
        rcInputs[i] = rawChannels[i] - config.rxChannelZeroOffset[i];
      }      
      
      rcTimeout = 0;
      overdue = false;
    }
  }

#if 0
  for (uint8_t i = 0; i < MAX_RC_CHANNELS; i++)
  {
    Serial.print(rcInputs[i]); Serial.print("\t");
  }
  Serial.println();
#endif

  // Normally MonopolarThrottle is referenced to the lowest throttle position.
  monopolarThrottle = rawChannels[THROTTLE] - config.rxChannelZeroOffset[THROTTLE]; 

  // Bipolar throttle must use the nominal mid-point as calibration is done at throttle minimum
  rcInputs[THROTTLE] = rawChannels[THROTTLE] - 0; 

  // Reverse primary channels as requested
  if (config.aileronPol == REVERSED)
  {
    rcInputs[AILERON] = -rcInputs[AILERON];
  }

  if (config.elevatorPol == REVERSED)
  {
    rcInputs[ELEVATOR] = -rcInputs[ELEVATOR];
  }

  if (config.rudderPol == REVERSED)
  {
    rcInputs[RUDDER] = -rcInputs[RUDDER];
  }

  // Calculate RX activity
  rxSum = rcInputs[AILERON] + rcInputs[ELEVATOR] + rcInputs[RUDDER];
  rxSumDiff = rxSum - oldRxSum;

  // Set RX activity flag if movement above noise floor or throttle above minimum
  if ((rxSumDiff > NOISE_THRESH) || (rxSumDiff < -NOISE_THRESH) || (monopolarThrottle > THROTTLEIDLE)) 
  {
    flightFlags |= (1 << RXACTIVITY);
  }
  else 
  {
    flightFlags &= ~(1 << RXACTIVITY);
  }
  
  // Preset RCinputs[NOCHAN] for sanity
  rcInputs[NOCHAN] = 0;

  oldRxSum = rxSum;
}

// Center sticks on request from Menu
void centerSticks(void)    
{
  uint8_t  i, j;
  int16_t  rxChannelZeroOffset[MAX_RC_CHANNELS] = {0,0,0,0,0,0,0,0};

  while (digitalRead(BUTTON4) == 0);
  
  // Take an average of eight readings
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < MAX_RC_CHANNELS; j++)
    {
      rxChannelZeroOffset[j] += rawChannels[j];
    }
  
    delay(100); // Wait for a new frame
  }

      
  for (i = 0; i < MAX_RC_CHANNELS; i++)
  {
    if (rxChannelZeroOffset[i] >= 0)
    {
      config.rxChannelZeroOffset[i] = ((rxChannelZeroOffset[i] + 4) >> 3); // Round and divide by 8
    }
    else
    {
      config.rxChannelZeroOffset[i] = ((rxChannelZeroOffset[i] - 4) >> 3); // Round and divide by 8
    }
  }

  saveConfigToEEPROM();
}

void updateTransition(void)
{
  int16_t temp = 0;
  
  // Offset RC input to (approx) -250 to 2250
  temp = rcInputs[config.flightChan] + 1000;

  // Trim lower end to zero (0 to 2250)
  if (temp < 0) temp = 0;

  // Convert 0 to 2250 to 0 to 125. Divide by 20
  // Round to avoid truncation errors
  transition = (temp + 10) / 20;

  // transition now has a range of 0 to 101 for 0 to 2000 input
  // Limit extent of transition value 0 to 100 (101 steps)
  if (transition > 100) transition = 100;
}

// Update channel order
void updateChOrder(void)
{
  uint8_t i;
  
  // Populate each channel number with the correct lookup channel
  for (i = 0; i < MAX_RC_CHANNELS; i++)
  {
    // Update channel sequence
    if (config.txSeq == FUTABASEQ)
    {
      config.channelOrder[i] = pgm_read_byte(&FUTABA[i]);
    }
    else if (config.txSeq == JRSEQ)
    {
      config.channelOrder[i] = pgm_read_byte(&JR[i]);
    }
    else  // Otherwise load from custom channel order
    {
      config.channelOrder[i] = config.customChannelOrder[i];
    }
  }
}

void serialEvent3(void)
{
  if (config.rxMode == SPEKTRUM)
  {
    while (Serial3.available()) 
    {
      dsm.handleSerialEvent(Serial3.read(), micros());
    }
  }
}
