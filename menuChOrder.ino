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

#define CHITEMS   8   // Number of menu items
#define CHOFFSET 70   // LCD offsets for TX number text
#define CHSTART  384  // Start of Menu text items (channel names)
#define CHTEXT   105  // Start of value text items (TX channel numbers)

//************************************************************
// RC menu items
//************************************************************

const uint16_t chMenuText[MAX_RC_CHANNELS] PROGMEM = 
{
  CHTEXT, CHTEXT, CHTEXT, CHTEXT, CHTEXT, CHTEXT, CHTEXT, CHTEXT
};
  
const uint16_t chMenuOffsets[MAX_RC_CHANNELS] PROGMEM = 
{
  CHOFFSET, CHOFFSET, CHOFFSET, CHOFFSET, CHOFFSET, CHOFFSET, CHOFFSET, CHOFFSET
};

const menuRange_t chMenuRanges[MAX_RC_CHANNELS] PROGMEM = 
{
  // Channels (8)
  {THROTTLE,AUX3,1,1,THROTTLE}, // Ch.1
  {THROTTLE,AUX3,1,1,AILERON},  // Ch.2
  {THROTTLE,AUX3,1,1,ELEVATOR}, // Ch.3
  {THROTTLE,AUX3,1,1,RUDDER},   // Ch.4
  {THROTTLE,AUX3,1,1,GEAR},     // Ch.5
  {THROTTLE,AUX3,1,1,AUX1},     // Ch.6
  {THROTTLE,AUX3,1,1,AUX2},     // Ch.7
  {THROTTLE,AUX3,1,1,AUX3},     // Ch.8
};

//************************************************************
// Channel menu-specific setup
//************************************************************

void menuChannel(void)
{
  int8_t *valuePtr;
  menuRange_t range;
  uint16_t reference = CHSTART;

  // If sub-menu item has changed, reset sub-menu positions
  if (menuFlag)
  {
    subTop = CHSTART;
    menuFlag = 0;
  }

  while (button != BACK)
  {
    valuePtr = &config.customChannelOrder[0];

    // Print menu
    printMenuItems(subTop, reference, valuePtr, (const unsigned char*)chMenuRanges, 0, (const uint16_t*)chMenuOffsets, (const uint16_t*)chMenuText, cursor);

    // Handle menu changes
    updateMenu(CHITEMS, reference, 0, button, &cursor, &subTop, &menuTemp);
    range = getMenuRange((const unsigned char*)chMenuRanges, (menuTemp - reference));

    if (button == ENTER)
    {
      doMenuItem(menuTemp, valuePtr + (menuTemp - reference), 1, range, 0, CHTEXT, false, 0);
    }

    // Update when exiting
    if (button == ENTER)
    {
      // Refresh channel order
      updateChOrder();
      
      saveConfigToEEPROM();  // Save value and return
      waitButton4();         // Wait for user's finger off the button
    }
  }
}
