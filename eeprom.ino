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

#define EEPROM_DATA_START_POS 0         // Make sure Rolf's signature is over-written for safety

// eePROM signature - change for each eePROM structure change to force factory reset or upgrade
#define V1_5_B3_SIGNATURE 0x44          // EEPROM signature for V1.5 (V1.5 Beta 3+)
#define V2p0_SIGNATURE    0x45          // EEPROM signature for V2.0 (teensyOAV with 10 outputs)

#define MAGIC_NUMBER V2p0_SIGNATURE     // Set current signature

//************************************************************
// Code
//************************************************************

const int8_t  JR[MAX_RC_CHANNELS]     PROGMEM = {0,1,2,3,4,5,6,7};  // JR/Spektrum channel sequence (TAERG123)
const int8_t  FUTABA[MAX_RC_CHANNELS] PROGMEM = {1,2,0,3,4,5,6,7};  // Futaba channel sequence (AETRGF12)
const int8_t  MPX[MAX_RC_CHANNELS]    PROGMEM = {1,2,3,5,0,4,6,7};  // Multiplex channel sequence (AER1TG23)
  
void saveConfigToEEPROM(void)
{
  // Write to eeProm
  cli();
  eepromWriteBlockChanges((uint8_t*)&config, (uint8_t*)EEPROM_DATA_START_POS, sizeof(CONFIG_STRUCT));  
  sei();
}

// src is the address in RAM
// dest is the address in eeprom
void eepromWriteBlockChanges(uint8_t *src, uint8_t *dest, uint16_t size)
{ 
  uint16_t len;
  uint8_t value;

  for (len = 0; len < size; len++)
  {
    // Get value at src
    value = *src;
    
    // Write the value at src to dest
    eepromWriteByteChanged(dest, value);
    src++;
    dest++;
  }
}

// addr is the address in eeprom
// value is the value to be written
void eepromWriteByteChanged(uint8_t *addr, uint8_t value)
{
  if (eeprom_read_byte(addr) != value)
  {
    eeprom_write_byte(addr, value);
  }
}

bool initialEEPROMConfigLoad(void)
{
  bool updated = false;
  
  // Read eeProm data into RAM
  eeprom_read_block((void*)&config, (const void*)EEPROM_DATA_START_POS, sizeof(CONFIG_STRUCT));
  
  // See if we know what to do with the current eeprom data
  // config.setup holds the magic number from the current EEPROM
  switch(config.setup)
  {
    case V2p0_SIGNATURE:  // V2.0
      break;
      
    default:              // Unknown solution - restore to factory defaults
      // Load factory defaults
      setEEPROMDefaultConfig();
      updated = true;
      break;
  }
  
  // Save back to eeprom  
  saveConfigToEEPROM();
  
  // Return info regarding eeprom structure changes 
  return updated;
}

// Force a factory reset
void setEEPROMDefaultConfig(void)
{
  uint8_t i;
  
  // Clear entire config space first
  memset(&config.setup,0,(sizeof(config)));

  // Set magic number to current signature
  config.setup = MAGIC_NUMBER;

  // General
  config.orientationP2   = UP_BACK;
  config.rxMode          = SBUS;
  config.flightChan      = GEAR;
  config.armMode         = ARMABLE;
  config.accLPF          = HZ21;
  config.gyroLPF         = NOFILTER;
  config.mpu6050LPF      = HZ44;
  config.cfFactor        = 6;
  config.disarmTimer     = 30;  // Default to 30 seconds
  config.transitionP1n   = 50;  // Set P1.n point to 50%
  config.transitionP1    = 0;
  config.transitionP2    = 100; 
  config.accelVertFilter = 20;
  config.buzzer          = ON;

  // Advanced
  config.orientationP1 = UP_BACK;
  config.p1Reference   = NO_ORIENT;
  
  // Preset AccZeroNormZ
  config.accelZeroNormZP1 = 128;
  config.accelZeroNormZP2 = 128;

  // Defaults
  for (i = 0; i < MAX_RC_CHANNELS; i++)
  {
    config.channelOrder[i]        = pgm_read_byte(&JR[i]);  // Preset channel order to JR
    config.customChannelOrder[i]  = pgm_read_byte(&JR[i]);  // Preset custom channel order to JR
    config.rxChannelZeroOffset[i] = 0;                      // Reset RX channel offsets
  }
  
  // Monopolar throttle is a special case. Set to -100% or -1000
  // Otherwise the throttle high alarm will go off on first power-up
  config.rxChannelZeroOffset[THROTTLE] = 0;

  // Preset mixers to safe values
  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    config.channel[i].p1SourceA = NOMIX;
    config.channel[i].p1SourceB = NOMIX;
    config.channel[i].p2SourceA = NOMIX;
    config.channel[i].p2SourceB = NOMIX;
    config.minTravel[i] = -100;
    config.maxTravel[i] =  100;
  }

  // Curves 0 and 1
  for (i = 0; i < 2; i++)
  {
    config.curve[i].point1 = 0;
    config.curve[i].point2 = 17;
    config.curve[i].point3 = 33;
    config.curve[i].point4 = 50;
    config.curve[i].point5 = 67;
    config.curve[i].point6 = 83;
    config.curve[i].point7 = 100;
  }

  // Curves 2 to 6  
  for (i = 2; i < 6; i++)
  {
    config.curve[i].point1 = -100;
    config.curve[i].point2 = -67;
    config.curve[i].point3 = -33;
    config.curve[i].point4 = 0;
    config.curve[i].point5 = 33;
    config.curve[i].point6 = 67;
    config.curve[i].point7 = 100;
  }

  config.curve[0].channel = THROTTLE;
  config.curve[1].channel = THROTTLE;
  config.curve[2].channel = THROTTLE;
  config.curve[3].channel = THROTTLE;
  config.curve[4].channel = NOMIX;
  config.curve[5].channel = NOMIX;
          
  // Load manual defaults
  loadEEPROMPreset(QUADX);
  config.preset = OPTIONS; // Menu will display "Options"
}

void loadEEPROMPreset(uint8_t preset)
{
  uint8_t i;

  // Erase current profile settings
  memset(&config.flightMode[P1],0,sizeof(flightControl_t));
  memset(&config.flightMode[P2],0,sizeof(flightControl_t));

  // Erase current mixer settings
  memset(&config.channel[OUT1],0,sizeof(channel_t) * MAX_OUTPUTS);

  // Preset mixers to safe values
  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    config.channel[i].p1SourceA = NOMIX;
    config.channel[i].p1SourceB = NOMIX;
    config.channel[i].p2SourceA = NOMIX;
    config.channel[i].p2SourceB = NOMIX;
    config.minTravel[i] = -100;
    config.maxTravel[i] =  100;
  }

  switch (preset)
  {
    case BLANK:
      config.armMode = ARMABLE;
      break;
    
    case QUADP:
      // Preset mixing for primary channels
      config.armMode = ARMABLE;
  
      // Preset AccVertFilter
      config.accelVertFilter = 20;

      // Preset Z-terms for all
      config.flightMode[P1].aZedPMult = 40;
      config.flightMode[P1].aZedIMult = 20;
      config.flightMode[P1].aZedLimit = 10;
      
      // Profile 1 (Baseline)
      config.flightMode[P1].rollPMult  = 50;
      config.flightMode[P1].rollIMult  = 10;
      config.flightMode[P1].rollLimit  = 10;
      config.flightMode[P1].rollRate   = 2;
      config.flightMode[P1].aRollPMult = 10;
      
      config.flightMode[P1].pitchPMult  = 50;
      config.flightMode[P1].pitchIMult  = 10;
      config.flightMode[P1].pitchLimit  = 10;
      config.flightMode[P1].pitchRate   = 2;
      config.flightMode[P1].aPitchPMult = 10;
      
      config.flightMode[P1].yawPMult = 60;
      config.flightMode[P1].yawIMult = 40;
      config.flightMode[P1].yawLimit = 25;
      config.flightMode[P1].yawRate  = 2;
      
      // Profile 2 (For comparison)
      config.flightMode[P2].rollPMult  = 40;
      config.flightMode[P2].rollIMult  = 19;
      config.flightMode[P2].rollLimit  = 14;
      config.flightMode[P2].rollRate   = 3;
      config.flightMode[P2].aRollPMult = 1;
      
      config.flightMode[P2].pitchPMult  = 40;
      config.flightMode[P2].pitchIMult  = 19;
      config.flightMode[P2].pitchLimit  = 14;
      config.flightMode[P2].pitchRate   = 3;
      config.flightMode[P2].aPitchPMult = 1;
      
      config.flightMode[P2].yawPMult = 60;
      config.flightMode[P2].yawIMult = 40;
      config.flightMode[P2].yawLimit = 25;
      config.flightMode[P2].yawRate  = 3;
  
      for (i = 0; i <= OUT4; i++)
      {
        config.channel[i].p1ThrottleVolume = 100;
        config.channel[i].p2ThrottleVolume = 100;
        config.channel[i].motorMarker      = MOTOR;
        config.channel[i].p1ZDeltaAccel    = ON;
        config.channel[i].p2ZDeltaAccel    = ON;
      }

      // OUT1
      config.channel[OUT1].p1AileronVolume  = 0;
      config.channel[OUT1].p2AileronVolume  = 0;
      config.channel[OUT1].p1ElevatorVolume = -20;
      config.channel[OUT1].p2ElevatorVolume = -30;
      config.channel[OUT1].p1RudderVolume   = -30;
      config.channel[OUT1].p2RudderVolume   = -40;
      config.channel[OUT1].p1PitchGyro      = ON;
      config.channel[OUT1].p1PitchAccel     = ON;
      config.channel[OUT1].p2PitchGyro      = ON;
      config.channel[OUT1].p2PitchAccel     = ON;
      config.channel[OUT1].p1YawGyro        = ON;
      config.channel[OUT1].p2YawGyro        = ON;
  
      // OUT2
      config.channel[OUT2].p1AileronVolume  = -20;
      config.channel[OUT2].p2AileronVolume  = -30;
      config.channel[OUT2].p1ElevatorVolume = 0;
      config.channel[OUT2].p2ElevatorVolume = 0;
      config.channel[OUT2].p1RudderVolume   = 30;
      config.channel[OUT2].p2RudderVolume   = 40;
      config.channel[OUT2].p1RollGyro       = ON;
      config.channel[OUT2].p1RollAccel      = ON;
      config.channel[OUT2].p2RollGyro       = ON;
      config.channel[OUT2].p2RollAccel      = ON;
      config.channel[OUT2].p1YawGyro        = ON;
      config.channel[OUT2].p2YawGyro        = ON;
  
      // OUT3
      config.channel[OUT3].p1AileronVolume  = 0;
      config.channel[OUT3].p2AileronVolume  = 0;
      config.channel[OUT3].p1ElevatorVolume = 20;
      config.channel[OUT3].p2ElevatorVolume = 30;
      config.channel[OUT3].p1RudderVolume   = -30;
      config.channel[OUT3].p2RudderVolume   = -40;
      config.channel[OUT3].p1PitchGyro      = ON;
      config.channel[OUT3].p1PitchAccel     = ON;
      config.channel[OUT3].p2PitchGyro      = ON;
      config.channel[OUT3].p2PitchAccel     = ON;
      config.channel[OUT3].p1YawGyro        = ON;
      config.channel[OUT3].p2YawGyro        = ON;
  
      // OUT4
      config.channel[OUT4].p1AileronVolume  = 20;
      config.channel[OUT4].p2AileronVolume  = 30;
      config.channel[OUT4].p1ElevatorVolume = 0;
      config.channel[OUT4].p2ElevatorVolume = 0;
      config.channel[OUT4].p1RudderVolume   = 30;
      config.channel[OUT4].p2RudderVolume   = 40;
      config.channel[OUT4].p1RollGyro       = ON;
      config.channel[OUT4].p1RollAccel      = ON;
      config.channel[OUT4].p2RollGyro       = ON;
      config.channel[OUT4].p2RollAccel      = ON;
      config.channel[OUT4].p1YawGyro        = ON;
      config.channel[OUT4].p2YawGyro        = ON;    

      // OUT5
      config.channel[OUT5].p1ElevatorVolume = 75;
      config.channel[OUT5].p2ElevatorVolume = 100;

      // OUT6
      config.channel[OUT6].p1AileronVolume = 75;
      config.channel[OUT6].p2AileronVolume = 100;

      // OUT7
      config.channel[OUT7].p1RudderVolume = 75;
      config.channel[OUT7].p2RudderVolume = 100;

      // OUT8
      config.offsets[OUT8].point1 = -100;
      config.offsets[OUT8].point2 = -67;
      config.offsets[OUT8].point3 = -33;
      config.offsets[OUT8].point4 = 0;
      config.offsets[OUT8].point5 = 33;
      config.offsets[OUT8].point6 = 67;
      config.offsets[OUT8].point7 = 100;  
      break;
        
    case QUADX:
      // Preset mixing for primary channels
      config.armMode = ARMABLE;

      // Preset AccVertFilter
      config.accelVertFilter = 20;

      // Preset Z-terms for all
      config.flightMode[P1].aZedPMult = 40;
      config.flightMode[P1].aZedIMult = 20;
      config.flightMode[P1].aZedLimit = 10;
      
      // Profile 1 (Baseline)
      config.flightMode[P1].rollPMult  = 40;
      config.flightMode[P1].rollIMult  = 10;
      config.flightMode[P1].rollLimit  = 10;
      config.flightMode[P1].rollRate   = 2;
      config.flightMode[P1].aRollPMult = 10;
  
      config.flightMode[P1].pitchPMult  = 40;
      config.flightMode[P1].pitchIMult  = 10;
      config.flightMode[P1].pitchLimit  = 10;
      config.flightMode[P1].pitchRate   = 2;
      config.flightMode[P1].aPitchPMult = 10;
  
      config.flightMode[P1].yawPMult = 60;
      config.flightMode[P1].yawIMult = 40;
      config.flightMode[P1].yawLimit = 25;
      config.flightMode[P1].yawRate  = 2;
  
      // Profile 2 (For comparison)
      config.flightMode[P2].rollPMult  = 40;
      config.flightMode[P2].rollIMult  = 19;
      config.flightMode[P2].rollLimit  = 14;
      config.flightMode[P2].rollRate   = 3;
      config.flightMode[P2].aRollPMult = 1;
  
      config.flightMode[P2].pitchPMult  = 40;
      config.flightMode[P2].pitchIMult  = 19;
      config.flightMode[P2].pitchLimit  = 14;
      config.flightMode[P2].pitchRate   = 3;
      config.flightMode[P2].aPitchPMult = 1;
  
      config.flightMode[P2].yawPMult = 60;
      config.flightMode[P2].yawIMult = 40;
      config.flightMode[P2].yawLimit = 25;
      config.flightMode[P2].yawRate  = 2;
  
      for (i = 0; i <= OUT4; i++)
      {
        config.channel[i].p1ThrottleVolume = 100;
        config.channel[i].p2ThrottleVolume = 100;
        config.channel[i].motorMarker      = MOTOR;
        config.channel[i].p1RollGyro       = ON;
        config.channel[i].p1RollAccel      = ON;
        config.channel[i].p2RollGyro       = ON;
        config.channel[i].p2RollAccel      = ON;
        config.channel[i].p1PitchGyro      = ON;
        config.channel[i].p1PitchAccel     = ON;
        config.channel[i].p2PitchGyro      = ON;
        config.channel[i].p2PitchAccel     = ON;
        config.channel[i].p1YawGyro        = ON;
        config.channel[i].p2YawGyro        = ON;
        config.channel[i].p1ZDeltaAccel    = ON;
        config.channel[i].p2ZDeltaAccel    = ON;
      }

      // OUT1
      config.channel[OUT1].p1AileronVolume  = 15;
      config.channel[OUT1].p2AileronVolume  = 20;
      config.channel[OUT1].p1ElevatorVolume = -15;
      config.channel[OUT1].p2ElevatorVolume = -20;
      config.channel[OUT1].p1RudderVolume   = -40;
      config.channel[OUT1].p2RudderVolume   = -50;
  
      // OUT2
      config.channel[OUT2].p1AileronVolume  = -15;
      config.channel[OUT2].p2AileronVolume  = -20;
      config.channel[OUT2].p1ElevatorVolume = -15;
      config.channel[OUT2].p2ElevatorVolume = -20;
      config.channel[OUT2].p1RudderVolume   = 40;
      config.channel[OUT2].p2RudderVolume   = 50;
  
      // OUT3
      config.channel[OUT3].p1AileronVolume  = -15;
      config.channel[OUT3].p2AileronVolume  = -20;
      config.channel[OUT3].p1ElevatorVolume = 15;
      config.channel[OUT3].p2ElevatorVolume = 20;
      config.channel[OUT3].p1RudderVolume   = -40;
      config.channel[OUT3].p2RudderVolume   = -50;
  
      // OUT4
      config.channel[OUT4].p1AileronVolume  = 15;
      config.channel[OUT4].p2AileronVolume  = 20;
      config.channel[OUT4].p1ElevatorVolume = 15;
      config.channel[OUT4].p2ElevatorVolume = 20;
      config.channel[OUT4].p1RudderVolume   = 40;
      config.channel[OUT4].p2RudderVolume   = 50;

      // OUT5
      config.channel[OUT5].p1ElevatorVolume = 75;
      config.channel[OUT5].p2ElevatorVolume = 100;
      
      // OUT6
      config.channel[OUT6].p1AileronVolume = 75;
      config.channel[OUT6].p2AileronVolume = 100;
            
      // OUT7
      config.channel[OUT7].p1RudderVolume = 75;
      config.channel[OUT7].p2RudderVolume = 100;
                  
      // OUT8
      config.offsets[OUT8].point1 = -100;
      config.offsets[OUT8].point2 = -67;
      config.offsets[OUT8].point3 = -33;
      config.offsets[OUT8].point4 = 0;
      config.offsets[OUT8].point5 = 33;
      config.offsets[OUT8].point6 = 67;
      config.offsets[OUT8].point7 = 100;
      break;
    
    case TRICOPTER:
      // Preset simple mixing for primary channels
      config.armMode = ARMABLE;

      // Preset AccVertFilter
      config.accelVertFilter = 20;

      // Preset Z-terms for all
      config.flightMode[P1].aZedPMult = 40;
      config.flightMode[P1].aZedIMult = 20;
      config.flightMode[P1].aZedLimit = 10;
          
      // Profile 1 (Baseline)
      config.flightMode[P1].rollPMult  = 40;
      config.flightMode[P1].rollIMult  = 10;
      config.flightMode[P1].rollLimit  = 10;
      config.flightMode[P1].rollRate   = 2;
      config.flightMode[P1].aRollPMult = 10;
      
      config.flightMode[P1].pitchPMult  = 40;
      config.flightMode[P1].pitchIMult  = 10;
      config.flightMode[P1].pitchLimit  = 10;
      config.flightMode[P1].pitchRate   = 2;
      config.flightMode[P1].aPitchPMult = 10;
      
      config.flightMode[P1].yawPMult = 60;
      config.flightMode[P1].yawIMult = 40;
      config.flightMode[P1].yawLimit = 25;
      config.flightMode[P1].yawRate  = 2;
      
      // Profile 2 (For comparison)
      config.flightMode[P2].rollPMult  = 40;
      config.flightMode[P2].rollIMult  = 19;
      config.flightMode[P2].rollLimit  = 14;
      config.flightMode[P2].rollRate   = 3;
      config.flightMode[P2].aRollPMult = 1;
      
      config.flightMode[P2].pitchPMult  = 40;
      config.flightMode[P2].pitchIMult  = 19;
      config.flightMode[P2].pitchLimit  = 14;
      config.flightMode[P2].pitchRate   = 3;
      config.flightMode[P2].aPitchPMult = 1;
      
      config.flightMode[P2].yawPMult = 60;
      config.flightMode[P2].yawIMult = 40;
      config.flightMode[P2].yawLimit = 25;
      config.flightMode[P2].yawRate  = 2;
    
      for (i = 0; i <= OUT3; i++)
      {
        config.channel[i].p1ThrottleVolume = 100;
        config.channel[i].p2ThrottleVolume = 100;
        config.channel[i].motorMarker      = MOTOR;
      }

      // OUT1
      config.channel[OUT1].p1AileronVolume  = 30;
      config.channel[OUT1].p2AileronVolume  = 40;
      config.channel[OUT1].p1ElevatorVolume = -15;
      config.channel[OUT1].p2ElevatorVolume = -20;
      config.channel[OUT1].p1RollGyro       = ON;
      config.channel[OUT1].p1RollAccel      = ON;
      config.channel[OUT1].p2RollGyro       = ON;
      config.channel[OUT1].p2RollAccel      = ON;
      config.channel[OUT1].p1PitchGyro      = SCALE;
      config.channel[OUT1].p1PitchAccel     = SCALE;
      config.channel[OUT1].p2PitchGyro      = SCALE;
      config.channel[OUT1].p2PitchAccel     = SCALE;
      config.channel[OUT1].p1ZDeltaAccel    = ON;
      config.channel[OUT1].p2ZDeltaAccel    = ON;
    
      // OUT2
      config.channel[OUT2].p1AileronVolume  = -30;
      config.channel[OUT2].p2AileronVolume  = -40;
      config.channel[OUT2].p1ElevatorVolume = -15;
      config.channel[OUT2].p2ElevatorVolume = -20;
      config.channel[OUT2].p1RollGyro       = ON;
      config.channel[OUT2].p1RollAccel      = ON;
      config.channel[OUT2].p2RollGyro       = ON;
      config.channel[OUT2].p2RollAccel      = ON;
      config.channel[OUT2].p1PitchGyro      = SCALE;
      config.channel[OUT2].p1PitchAccel     = SCALE;
      config.channel[OUT2].p2PitchGyro      = SCALE;
      config.channel[OUT2].p2PitchAccel     = SCALE;
      config.channel[OUT2].p1ZDeltaAccel    = ON;
      config.channel[OUT2].p2ZDeltaAccel    = ON;
    
      // OUT3
      config.channel[OUT3].p1ElevatorVolume = 30;
      config.channel[OUT3].p2ElevatorVolume = 40;
      config.channel[OUT3].p1PitchGyro      = SCALE;
      config.channel[OUT3].p1PitchAccel     = SCALE;
      config.channel[OUT3].p2PitchGyro      = SCALE;
      config.channel[OUT3].p2PitchAccel     = SCALE;
      config.channel[OUT3].p1ZDeltaAccel   = ON;
      config.channel[OUT3].p2ZDeltaAccel   = ON;
      
      // OUT4
      config.channel[OUT4].motorMarker    = ASERVO;
      config.channel[OUT4].p1RudderVolume = 75;
      config.channel[OUT4].p2RudderVolume = 100;
      config.channel[OUT4].p1YawGyro      = ON;
      config.channel[OUT4].p2YawGyro      = ON;
      
      // OUT5
      config.channel[OUT5].p1ElevatorVolume = 75;
      config.channel[OUT5].p2ElevatorVolume = 100;

      // OUT6
      config.channel[OUT6].p1AileronVolume = 75;
      config.channel[OUT6].p2AileronVolume = 100;

      // OUT7
      config.channel[OUT7].p1RudderVolume = 75;
      config.channel[OUT7].p2RudderVolume = 100;

      // OUT8
      config.offsets[OUT8].point1 = -100;
      config.offsets[OUT8].point2 = -67;
      config.offsets[OUT8].point3 = -33;
      config.offsets[OUT8].point4 = 0;
      config.offsets[OUT8].point5 = 33;
      config.offsets[OUT8].point6 = 67;
      config.offsets[OUT8].point7 = 100;
      break;
    
    default:
      break;
  }
}
