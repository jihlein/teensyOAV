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

#define RCITEMSOFFSET 12  // Max number of menu items

#define RCOFFSET      60  // LCD offsets
#define RCSTART      146  // Start of Menu text items
#define RCTEXT       396  // Start of "Receiver type" value text list

#define GENERALTEXT  295  // Start of "Orientations" value text list
#define GENOFFSET     70  // LCD offsets

#define PRESETITEM   171  // Location of Preset menu item in list

//************************************************************
// RC menu items
//************************************************************
   
const uint16_t rcMenuText[2][RCITEMSOFFSET] PROGMEM = 
{
  {RCTEXT, 130, 105, 0, 0, 0, 0, 0, 68, 0},              // RC setup
  {GENERALTEXT, 320, 53, 0, 0, 37, 37, 37, 0, 273, 68},  // General
};

const uint16_t rcMenuOffsets[2][RCITEMSOFFSET] PROGMEM =
{
  {RCOFFSET, 60, 75, 95, 95, 95, 95, 95, 95, 95},   // RC setup
  {GENOFFSET, GENOFFSET, GENOFFSET, 80, 80, 80, 80, 80, 80, 80, 80},  // General
};

const menuRange_t rcMenuRanges[2][RCITEMSOFFSET] PROGMEM = 
{
  {
    // RC setup (12)                // Min, Max, Increment, Style, Default
    {SBUS, CPPM_MODE, 1 ,1, SBUS},  // Receiver type (SBUS to CPPM)  
    {JRSEQ, CUSTOM, 1, 1, JRSEQ},   // Channel order
    {THROTTLE ,AUX3, 1, 1, GEAR},   // Profile select channel
    {0, 40, 1, 0, 0},               // Outbound TransitionSpeed 0 to 40
    {0, 40, 1, 0, 0},               // Inbound TransitionSpeed 0 to 40
    {0, 99, 1, 0, 0},               // Transition P1 point
    {1, 99, 1 ,0, 50},              // Transition P1n point
    {1, 100, 1, 0, 100},            // Transition P2 point
    {OFF, ON, 1, 1, OFF},           // Vibration display
    {0, 127, 1, 0, 20},             // AccVert filter in 1/100th %
  },
  {
    // General (12)
    {UP_BACK, RIGHT_FRONT, 1, 1, UP_BACK},  // Orientation (P2)
    {NO_ORIENT, MODEL, 1, 1, NO_ORIENT},    // Orientation usage (Tail sitter)
    {ARMED, ARMABLE, 1, 1, ARMABLE},        // Arming mode Armable/Armed
    {0, 127, 1, 0, 30},                     // Auto-disarm enable
    {0, 8, 1, 1, 0},                        // Low battery cell voltage
    {HZ5, HZ260, 1, 1, HZ44},               // MPU6050 LPF. Default is 44Hz
    {HZ5, NOFILTER, 1, 1, HZ21},            // Acc. LPF 21Hz default  (5, 10, 21, 44, 94, 184, 260, None)
    {HZ5, NOFILTER, 1, 1, NOFILTER},        // Gyro LPF. No LPF default (5, 10, 21, 44, 94, 184, 260, None)
    {2, 11 ,1, 0, 6},                       // AL correction
    {QUADX, BLANK, 1, 4, QUADX},            // Mixer preset (note: style 4)
    {OFF, ON, 1, 1, ON},                    // Buzzer ON/OFF
  }
};

// These are the implied P1 orientations based on the user's P2 orientation.
const int8_t p1OrientationLUT[NUMBEROFORIENTS] PROGMEM = 
  {
    BACK_DOWN,   BACK_LEFT,   BACK_UP,    BACK_RIGHT,
    DOWN_FRONT,  DOWN_LEFT,   DOWN_BACK,  DOWN_RIGHT,
    FRONT_DOWN,  FRONT_RIGHT, FRONT_UP,   FRONT_LEFT,
    UP_FRONT,    UP_RIGHT,    UP_BACK,    UP_LEFT,
    LEFT_FRONT,  LEFT_UP,     LEFT_BACK,  LEFT_DOWN,
    RIGHT_FRONT, RIGHT_DOWN,  RIGHT_BACK, RIGHT_UP
  };

//************************************************************
// Main menu-specific setup
//************************************************************

void menuRcSetup(uint8_t section)
{
  int8_t      *valuePtr = &config.rxMode;
  int8_t      currentRxMode;
  menuRange_t range;
  uint16_t    textLink;
  uint16_t    offset = 0;
  uint16_t    items;
  
  // If submenu item has changed, reset submenu positions
  if (menuFlag)
  {
    subTop = RCSTART;
    menuFlag = 0;
  }

  while(button != BACK)
  {
    // Get menu offsets and load values from eeprom
    // 1 = RC, 2 = General, 3 = Advanced
    switch(section)
    {
      case 1:        // RC setup menu
        items = 10;  // Number of RC setup menu lines
        break;
      
      case 2:           // General menu
        offset   = RCITEMSOFFSET;
        items    = 11;  // Number of general menu lines
        valuePtr = &config.orientationP2;
        break;
      
      default:
        break;
    }

    // Save rx mode in case of change requiring reboot
    currentRxMode = config.rxMode;
    
    // Always show preset text as "Options", regardless of actual setting
    config.preset = OPTIONS;

    // Print menu - note that print_menu_items() updates button variable.
    printMenuItems(subTop + offset, RCSTART + offset, valuePtr, (const unsigned char*)rcMenuRanges[section - 1], 0, (const uint16_t*)rcMenuOffsets[section - 1], (const uint16_t*)rcMenuText[section - 1], cursor);

    // Handle menu changes
    updateMenu(items, RCSTART, offset, button, &cursor, &subTop, &menuTemp);
    range = getMenuRange ((const unsigned char*)rcMenuRanges[section - 1], (menuTemp - RCSTART - offset)); 

    // If actually editing the preset, show the default setting
    if (menuTemp == PRESETITEM)
    {
      config.preset = QUADX;      
    }

    if (button == ENTER)
    {
      textLink = pgm_read_word(&rcMenuText[section - 1][menuTemp - RCSTART - offset]);
      doMenuItem(menuTemp, valuePtr + (menuTemp - RCSTART - offset), 1, range, 0, textLink, false, 0);
    }

    // Handle abort neatly
    if (button == ABORT)
    {
      waitButton1();      // Wait for user's finger off the button
      button = NONE;
      
      // Reset the mixer preset if unchanged
      config.preset = OPTIONS;
    }

    // Post-processing on exit
    if (button == ENTER)
    {
      updateLimits();     // Update I-term limits and triggers based on percentages
      
      // See if mixer preset has changed. Load new preset only if so
      if ((config.preset != OPTIONS) && (menuTemp == PRESETITEM))
      {
        loadEEPROMPreset(config.preset);
      }

      // Update MPU6050 LPF and reverse sense of menu items
      mpu.setDLPFMode(6 - config.mpu6050LPF);

      // Refresh channel order
      updateChOrder();

      if (config.armMode == ARMABLE)
      {
        generalError |= (1 << DISARMED); // Set flags to disarmed
        LED_OFF;
      }

      // Work out the P1 orientation from the user's P2 orientation setting
      config.orientationP1 = (int8_t)pgm_read_byte(&p1OrientationLUT[config.orientationP2]);

      // Return the preset to "Options" when going back to the General menu
      config.preset = OPTIONS;

      saveConfigToEEPROM(); // Save values

      // In case rxMode changed, reset to re-init rx pin/library
      if (currentRxMode != config.rxMode)
      {
        SCB_AIRCR = 0x05FA0004;
        while(1);  // Wait for reboot 
      }
      
      // In case preset mode changed, re-setup the servo output types
      setupServos();
      
      waitButton4();  // Wait for user's finger off the button
    }
  }
}
