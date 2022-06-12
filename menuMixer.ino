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

#define MIXERITEMS   30  // Number of mixer menu items
#define MIXERSTARTE 192  // Start of Menu text items (Earth)
#define MIXERSTARTM 348  // Start of Menu text items (Model)
#define MIXOFFSET    85  // Value offsets

//************************************************************
// RC menu items
//************************************************************
   
const uint16_t mixerMenuTextE[MIXERITEMS] PROGMEM =
{
  225,0,0,56,                           // Motor control and offsets (4)
  0,0,0,0,0,0,                          // Flight controls (6)
  68,68,68,68,68,68,68,68,68,68,68,68,  // Mixer ranges (12)
  421,0,421,0,421,0,421,0               // Other sources (8)
};

const uint16_t mixerMenuTextM[MIXERITEMS] PROGMEM =
{
  225,0,0,56,                           // Motor control and offsets (4)
  0,0,0,0,0,0,                          // Flight controls (6)
  68,68,68,68,68,68,68,68,68,68,68,68,  // Mixer ranges (12)
  442,0,421,0,442,0,421,0               // Other sources (8)
};

const uint16_t mixerMenuOffsets[MIXERITEMS] PROGMEM =
{
  MIXOFFSET,92,92,92,                   // Motor control and offsets (4)
  92,92,92,92,92,92,                    // Flight controls (6)
  92,92,92,92,92,92,92,92,92,92,92,92,  // Mixer ranges (12)
  77,77,77,77,77,77,77,77               // Other sources (8)
};

const menuRange_t mixerMenuRanges[MIXERITEMS] PROGMEM = 
{
    // Motor control and offsets (4)
    {ASERVO,ONESHOT,1,1,ASERVO},   // Motor marker (0)
    {0,125,1,0,100},               // P1 throttle volume 
    {0,125,1,0,100},               // P2 throttle volume
    {LINEAR,SQRTSINE,1,1,LINEAR},  // Throttle curves

    // Flight controls (6)
    {-125,125,1,0,0},              // P1 Aileron volume (8)
    {-125,125,1,0,0},              // P2 Aileron volume
    {-125,125,1,0,0},              // P1 Elevator volume
    {-125,125,1,0,0},              // P2 Elevator volume
    {-125,125,1,0,0},              // P1 Rudder volume
    {-125,125,1,0,0},              // P2 Rudder volume

    // Mixer ranges (12)
    {OFF, SCALE,1,1,OFF},          // P1 roll_gyro (14)
    {OFF, SCALE,1,1,OFF},          // P2 roll_gyro
    {OFF, SCALE,1,1,OFF},          // P1 pitch_gyro
    {OFF, SCALE,1,1,OFF},          // P2 pitch_gyro
    {OFF, SCALE,1,1,OFF},          // P1 yaw_gyro
    {OFF, SCALE,1,1,OFF},          // P2 yaw_gyro
    {OFF, SCALE,1,1,OFF},          // P1 roll_acc
    {OFF, SCALE,1,1,OFF},          // P2 roll_acc
    {OFF, SCALE,1,1,OFF},          // P1 pitch_acc
    {OFF, SCALE,1,1,OFF},          // P2 pitch_acc
    {OFF, SCALE,1,1,OFF},          // P1 Z_delta_acc
    {OFF, SCALE,1,1,OFF},          // P2 Z_delta_acc

    // Sources (8)
    {SRC2,NOMIX,1,1,NOMIX},        // P1 Source A (26)
    {-125,125,1,0,0},              // P1 Source A volume
    {SRC2,NOMIX,1,1,NOMIX},        // P2 Source A
    {-125,125,1,0,0},              // P2 Source A volume
    {SRC2,NOMIX,1,1,NOMIX},        // P1 Source B
    {-125,125,1,0,0},              // P1 Source B volume
    {SRC2,NOMIX,1,1,NOMIX},        // P2 Source B
    {-125,125,1,0,0},              // P2 Source B volume
};

//************************************************************
// Main menu-specific setup
//************************************************************

void menuMixer(uint8_t i)
{
  int8_t   *valuePtr;
  menuRange_t range;
  uint16_t textLink = 0;
  uint16_t reference;

  // Set the correct text list for the selected reference
  if (config.p1Reference != MODEL)
  {
    reference = MIXERSTARTE;
  }
  else
  {
    reference = MIXERSTARTM;
  }
  
  // If sub-menu item has changed, reset sub-menu positions
  if (menuFlag)
  {
    // Set the correct text list for the selected reference
    if (config.p1Reference != MODEL)
    {
      subTop = MIXERSTARTE;
    }
    else
    {
      subTop = MIXERSTARTM;
    }
    menuFlag = 0;
  }

  while (button != BACK)
  {
    valuePtr = &config.channel[i].motorMarker;

    // Print menu
    // Set the correct text list for the selected reference
    if (config.p1Reference != MODEL)
    {
      printMenuItems(subTop, reference, valuePtr, (const unsigned char*)mixerMenuRanges, 0, (const uint16_t*) mixerMenuOffsets, (const uint16_t*)mixerMenuTextE, cursor);
    }
    else
    {
      printMenuItems(subTop, reference, valuePtr, (const unsigned char*)mixerMenuRanges, 0, (const uint16_t*) mixerMenuOffsets, (const uint16_t*)mixerMenuTextM, cursor);
    }

    // Handle menu changes
    updateMenu(MIXERITEMS, reference, 0, button, &cursor, &subTop, &menuTemp);
    range = getMenuRange ((const unsigned char*)mixerMenuRanges, menuTemp - reference);

    if (button == ENTER)
    {
      // Set the correct text list for the selected reference
      if (config.p1Reference != MODEL)
      {
        textLink = pgm_read_word(&mixerMenuTextE[menuTemp - reference]);
      }
      else
      {
        textLink = pgm_read_word(&mixerMenuTextM[menuTemp - reference]);
      }
      
      doMenuItem(menuTemp, valuePtr + (menuTemp - reference), 1, range, 0, textLink, false, 0);
    }

    // Update limits when exiting
    if (button == ENTER)
    {
      updateLimits();        // Update travel limits based on percentages
      saveConfigToEEPROM();  // Save value and return
      setupServos();         // Re-setup servos in case output type was changed
      waitButton4();         // Wait for user's finger off the button
    }
  }
}
