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

// Hard-coded Line Positions
const uint8_t lines[4] PROGMEM = {LINE0, LINE1, LINE2, LINE3};

// Defines
#define CURVESTARTE 421
#define CURVESTARTM 442

//************************************************************
// Print basic menu frame
// style = menu style
//************************************************************

void printMenuFrame(uint8_t style)
{
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  u8g2.drawStr( 38, 55, "\x70");  // Up
  u8g2.drawStr( 80, 55, "\x6D");  // Down
  u8g2.setFont(u8g2_font_helvR08_tr);  
      
  switch (style)
  {
    case BASIC:
      // Print bottom markers
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 55, "\x6E");  // Left
      u8g2.drawStr(120, 55, "\x6F");  // Right
      u8g2.setFont(u8g2_font_helvR08_tr);  
      break;

    case EDIT:
      // For editing items
      lcdDisplayText(16,   0, 54);  // Def.
      lcdDisplayText(17, 103, 54);  // Save
      break;
      
    case ABORT:
      // Save or abort
      lcdDisplayText(280  , 0, 54);  // Abort
      lcdDisplayText( 17, 103, 54);  // Save
      break;
      
    case LOG:
      // Clear or exit
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 55, "\x6E");  // Left
      u8g2.setFont(u8g2_font_helvR08_tr);  
      lcdDisplayText(291, 98, 54);    // Clear
      break;      

    case CURVE:
      // Curve edit screen
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 56, "\x6E");  // Left
      u8g2.drawStr(120, 56, "\x6F");  // Right
      u8g2.setFont(u8g2_font_helvR08_tr);  
      break;

    case OFFSET:
      // Offset curve edit screen
      u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
      u8g2.drawStr(  0, 56, "\x6E");  // Left
      u8g2.drawStr(120, 56, "\x6F");  // Right
      u8g2.setFont(u8g2_font_helvR08_tr);  
      lcdDisplayText(48,  10, 54);    // P1
      lcdDisplayText(49,  55, 54);    // P1.n 
      lcdDisplayText(50, 107, 54);    // P2
      break;    
      
    default:
      break;

  }
}

//**********************************************************************
// Print menu items primary subroutine
//
// Usage:
// top = position in sub-menu list
// start = start of sub-menu text list. (top - start) gives the offset into the list.
// values = pointer to array of values to change
// menuRanges = pointer to array of min/max/inc/style/defaults
// rangeType = unique (0) all values are different, copied (1) all values are the same
// menuOffsets = originally an array, now just a fixed horizontal offset for the value text
// textLink = pointer to the text list for the values if not numeric
// cursor = cursor position
//**********************************************************************

void printMenuItems(uint16_t top, uint16_t start, int8_t values[], const unsigned char* menuRanges, uint8_t rangeType, const uint16_t* menuOffsets, const uint16_t* textLink, uint16_t cursor)
{
  menuRange_t range1;
  uint16_t base = 0;
  uint16_t offset = 0;
  uint16_t text = 0;
  uint8_t textOffset = 0;
    
  u8g2.clearBuffer();
  printMenuFrame(BASIC);
  
  // Print each line
  for (uint8_t i = 0; i < 4; i++)
  {
    lcdDisplayText(top + i, ITEMOFFSET, (uint8_t)pgm_read_byte(&lines[i]));

    // Handle unique or copied ranges (to reduce space)
    if (rangeType == 0)
    {
      // Use each unique entry
      memcpy_P(&range1, &menuRanges[(top + i - start)* sizeof(range1)], sizeof(range1));
    }
    else
    {
      // Use just the first entry in array for all 
      memcpy_P(&range1, &menuRanges[0], sizeof(range1));
    }

    // Calculate location of text to display
    base = pgm_read_word(&textLink[top + i - start]);
    offset = values[top + i - start];
    text = base + offset;

    // Calculate horizontal offset of text to display
    textOffset = (uint8_t)pgm_read_word(&menuOffsets[top + i - start]);
  
    printMenuText((values[top + i - start]), range1.style, text, textOffset, (uint8_t)pgm_read_byte(&lines[i]));
  }

  printCursor(cursor);
  u8g2.sendBuffer();
  pollButtons(true);
}

//************************************************************
// getMenuRange - Get range info from PROGMEM for a specific item
//************************************************************

menuRange_t getMenuRange(const unsigned char* menuRanges, uint8_t menuItem)
{
  menuRange_t range;
  memcpy_P(&range, &menuRanges[menuItem * sizeof(range)], sizeof(range));
  return(range);
}

//************************************************************
// Edit current value according to limits and increment
// menuitem = Item reference
// values = pointer to value to change
// multiplier = display/actual
// range   = Limits of item
// offset = Horizontal offset on screen
// text_link = Start of text list for the values if not numeric
// servo_enable = Enable real-time updating of servo position
// servo_number = Servo number to update
//************************************************************

void doMenuItem(uint16_t menuItem, int8_t *values, uint8_t mult, menuRange_t range, int8_t offset, uint16_t textLink, bool servoEnable, int16_t servoNumber)
{
  int16_t temp16;
  int16_t value = (int8_t)*values;
  uint8_t displayUpdate = 0;
  uint8_t servoUpdate   = 0;
  uint8_t buttonUpdate  = 0;
  uint8_t buttonInc     = 0;
  bool    buttonLock    = false;
  bool    firstTime     = true;

  // Multiply value for display only if style is 2
  if (range.style == 2)
  {
    value = value * mult;
  }
  else mult = 1;

  button = NONE;

  // This is a loop that cycles until Button 4 is pressed (Save) or an Abort button
  // The GLCD updating slows servo updates down too much so only update the GLCD periodically
  // When not updating the GLCD the servo should be updated at 50Hz (20ms)
  while ((button != ENTER) && (button != ABORT))
  {
    // Increment loop count so that we can time various things
    displayUpdate++;
    servoUpdate++;

    // Vary the button increment delay depending on the function
    if (servoEnable)
    {
      buttonInc = 5; // For servos
    }
    else
    {
      buttonInc = 1; // For everything else (numbers)
    }

    // Increment button timer when pressed
    if (button != NONE)
    {
      buttonUpdate++;

      // Release button lock after button_inc loops
      if (buttonUpdate > buttonInc)
      {
        buttonLock = false;
        buttonUpdate = 0;
      } 
    }
    // Remove lock when not pressed
    else 
    {
      buttonUpdate = 0;
      buttonLock = false;
    }

    // Display update
    if (!servoEnable ||                               // Non-servo value or
       ((displayUpdate >= 8) && (button != NONE)) ||  // Servo value and 8 cycles passed but only with a button pressed or...
       (firstTime))                                   // First time into routine
    {
      displayUpdate = 0;
      firstTime = false;

      u8g2.clearBuffer();

      // Print warning
      if (range.style == 4)
      {
        lcdDisplayText(281, 0, 0);  // Warning
        lcdDisplayText(282, 25, 12);
      }
      // Print title
      else
      {
        lcdDisplayText(menuItem, 0, 0);
      }

      // Print value
      if ((range.style == 0) || (range.style == 2) || (range.style == 3))  // numeric, numeric * 4, servo limits
      {
        // Write numeric value, centered on screen
        itoa(value, pBuffer, 10);
        uint8_t strWidth = u8g2.getStrWidth(pBuffer);
        u8g2.setCursor(((128 - strWidth) / 2) + offset, 25); 
        u8g2.println(pBuffer);
      }
      else // text (style 1 or 4)
      {
        // Write text, centered on screen
        uint8_t strWidth = u8g2.getStrWidth((char*)(textMenu[textLink + value]));
        lcdDisplayText(textLink + value, ((128 - strWidth) / 2), 25);
      }

      // Print appropriate menu frame
      // Save/Abort screen
      if (range.style == 4)
      {
        // Print bottom markers
        printMenuFrame(ABORT);        
      }
      // Save/default screen
      else
      {
        // Print bottom markers
        printMenuFrame(EDIT);       
      }

      // Write from buffer
      u8g2.sendBuffer();
    }
  
    // Slow the loop rate more for text items (1 and 4) less for servos (3)
    switch (range.style)
    {
      case 0:
        // Loop rate = 50ms (10Hz)
        delay(100);
        break;
      case 1:
        // Loop rate = 250ms (4Hz)
        delay(250);   
        break;
      case 2:
        // Loop rate = 50ms (10Hz)
        delay(100);
        break;
      case 3:       
        // Loop rate = 20ms (50Hz)
        delay(20);    
        break;    
      case 4:
        // Loop rate = 250ms (4Hz)
        delay(250); 
        break;    
    }

    // Poll buttons when idle.
    // Don't use button acceleration when moving servos
    // And don't block the code with poll_buttons()
    if (servoEnable)
    {
      button = (pinb & 0xF0); 
      buttonMultiplier = 1;
    }
    else
    {
      pollButtons(true);
    }

    // Release button lock when pressed
    // unless a servo
    if ((button != NONE) && (!servoEnable))
    {
      buttonLock = false;
    }
    
    // Handle cursor Up/Down limits
    if (button == DOWN)
    {
      if (buttonLock == false)
      {
        buttonLock = true;
        value = value - (range.increment * buttonMultiplier);
        buttonUpdate = 0;
      }
    }

    if (button == UP)
    {
      if (buttonLock == false)
      {
        buttonLock = true;
        value = value + (range.increment * buttonMultiplier);
        buttonUpdate = 0;
      }
    }

    // Handle button 1
    if (button == BACK) 
    {
      // Save/Abort screen
      if (range.style == 4)
      {
        button = ABORT;
      }
      else
      {
        value = (range.defaultValue * mult);       
      }
    }

    // Limit values to set ranges
    if (value < (range.lower * mult)) 
    {
      value = range.lower * mult;
    }
    
    if (value > (range.upper * mult)) 
    {
      value = range.upper * mult;
    }

    // Set servo position if required
    // Ignore if the output is marked as a motor
    if  ((servoEnable) && (config.channel[servoNumber].motorMarker != MOTORPWM && config.channel[servoNumber].motorMarker != ONESHOT))
    {
      servoUpdate = 0;

      temp16 = scalePercent(value);  // Convert to servo position (from %)

      // Scale motor from 2500~5000 to 1000~2000
      servoCmd[servoNumber] = ((temp16 << 2) + 5) / 10;  // Round and convert

      send250HzServos();
      send50HzServos();
    }
  } 

  // Divide value from that displayed if style = 2
  if (range.style == 2)
  {
    value = value / mult;
  }

  *values = (int8_t)value;
}

//************************************************************
// Edit current curve points according to limits and increment
// curve        = curve number to edit
// type         = CURVE or OFFSET
// Curves_menu_ranges = pointer to list of ranges for this curve
//************************************************************

void editCurveItem(uint8_t curve, uint8_t type)
{
  int16_t  value = 0;
  uint8_t  pointRef = 0;
  menuRange_t range;
  int16_t  points[NUMBEROFPOINTS];
  int8_t   interPoints[NUMBEROFPOINTS];
  int8_t   i = 0;
  int8_t   pointX = 0;
  int8_t   pointY = 0;
  int8_t   varBoxX = 0;
  int8_t   varBoxY = 0;
  int8_t   chanBoxY = 0; 
  int8_t   channel = THROTTLE;
  uint16_t reference = CURVESTARTE;
  
  button = NONE;

  // Set the correct text list for the selected reference
  if (config.p1Reference != MODEL)
  {
    reference = CURVESTARTE;
  }
  else
  {
    reference = CURVESTARTM;
  }

  // This is a loop that cycles until Button 4 is pressed (Save) or an Abort button
  while ((button != ENTER) && (button != ABORT))
  {
    // Handle offset curves differently
    if (type == OFFSET)
    {
      // Get curve point ranges
      range = getMenuRange((const unsigned char*)offsetsMenuRanges[curve], pointRef);
    }
    else
    {
      range = getMenuRange((const unsigned char*)curvesMenuRanges[curve], pointRef);
    }

    u8g2.clearBuffer();
    
    // Handle offset curves differently
    if (type == OFFSET)
    {
      // Print graph frame
      printMenuFrame(OFFSET);
    }
    else
    {
      // Print graph frame
      printMenuFrame(CURVE);
    }
    
    // Print axes
    if (type == OFFSET)
    {
      u8g2.drawLine(64, 0, 64, 52);  // Vertical
    }
    else
    {
      u8g2.drawLine(64, 0, 64, 57);  // Vertical
    }
    
      u8g2.drawLine(0, 29, 128, 29);  // Horizontal
        
    // Get the current curve's data
    if (type == OFFSET)
    {
      points[0] = config.offsets[curve].point1;
      points[1] = config.offsets[curve].point2;
      points[2] = config.offsets[curve].point3;
      points[3] = config.offsets[curve].point4;
      points[4] = config.offsets[curve].point5;
      points[5] = config.offsets[curve].point6;
      points[6] = config.offsets[curve].point7;
      channel   = config.offsets[curve].channel;
    }
    else
    {
      points[0] = config.curve[curve].point1;
      points[1] = config.curve[curve].point2;
      points[2] = config.curve[curve].point3;
      points[3] = config.curve[curve].point4;
      points[4] = config.curve[curve].point5;
      points[5] = config.curve[curve].point6;
      points[6] = config.curve[curve].point7;
      channel   = config.curve[curve].channel;      
    }

    // Calculate and draw points
    for (i = 0; i < 7; i++)
    {
      // Interpolate points for the offset graph
      if (type == CURVE)
      {
        // Curves 0 to 100
        if (curve < 2)
        {
          interPoints[i] = (int8_t)(54 - ((points[i] * 50) / 100));       
        }
        // Curves -100 to -100
        else
        {
          interPoints[i] = (int8_t)(29 - ((points[i] * 50) / 200));     
        }
      }
      // Curves -125 to -125
      else
      {
        interPoints[i] = (int8_t)(29 - ((points[i] * 50) / 250));
      }

      // Draw boxes on the five points
      switch(i)
      {
        case 0:
          pointX = 2;
          break;
        
        case 1:
          pointX = 22;
          break;
        
        case 2:
          pointX = 42;
          break;
        
        case 3:
          pointX = 62;
          break;
        
        case 4:
          pointX = 82;
          break;
        
        case 5:
          pointX = 102;
          break;
        
        case 6:
          pointX = 121;
          break;
      }
      
      // White box surrounding point (vertical origin is offset by 2)
      u8g2.drawBox(pointX, interPoints[i] - 2, 5, 5);
    }

    // Draw lines between the points
    u8g2.drawLine(  4, interPoints[0],  24, interPoints[1]);
    u8g2.drawLine( 24, interPoints[1],  44, interPoints[2]);
    u8g2.drawLine( 44, interPoints[2],  64, interPoints[3]);
    u8g2.drawLine( 64, interPoints[3],  84, interPoints[4]);
    u8g2.drawLine( 84, interPoints[4], 104, interPoints[5]);
    u8g2.drawLine(104, interPoints[5], 123, interPoints[6]);

    // Highlight the current point
    switch(pointRef)
    {
      case 0:
        pointX = 0;
        break;
      
      case 1:
        pointX = 20;
        break;
      
      case 2:
        pointX = 40;
        break;
      
      case 3:
        pointX = 60;
        break;
      
      case 4:
        pointX = 80;
        break;
      
      case 5:
        pointX = 100;
        break;
      
      case 6:
        pointX = 119;
        break;
      
      case 7:
        pointX = 119;
        break;
    }
    
    // Adjust box coordinates
    pointY = (interPoints[pointRef] - 4);

    // Channel numbers are highlighted differently
    if (pointRef == 7)
    {
      uint8_t x = u8g2.getStrWidth((char*)(textMenu[reference + config.curve[curve].channel]));  // Copy string to pBuffer
      uint8_t y = 10;
      u8g2.drawFrame((122 - x),(chanBoxY - y), (x + 6), (y + 4));
    }
    else
    {
      u8g2.drawFrame(pointX, pointY, 9, 9);
    }

    // Print value of current object in a box somewhere
    if (type == CURVE)
    {
      switch(pointRef)
      {
        case 0:
          value = config.curve[curve].point1;
          break;
        
        case 1:
          value = config.curve[curve].point2;
          break;
        
        case 2:
          value = config.curve[curve].point3;
          break;
        
        case 3:
          value = config.curve[curve].point4;
          break;
        
        case 4:
          value = config.curve[curve].point5;
          break;
        
        case 5:
          value = config.curve[curve].point6;
          break;
        
        case 6:
          value = config.curve[curve].point7;
          break;
        
        case 7:
          value = config.curve[curve].channel;
          break;
      }
    }
    // Offsets
    else
    {
      switch(pointRef)
      {
        case 0:
          value = config.offsets[curve].point1;
          break;
        
        case 1:
          value = config.offsets[curve].point2;
          break;
        
        case 2:
          value = config.offsets[curve].point3;
          break;
        
        case 3:
          value = config.offsets[curve].point4;
          break;
        
        case 4:
          value = config.offsets[curve].point5;
          break;
        
        case 5:
          value = config.offsets[curve].point6;
          break;
        
        case 6:
          value = config.offsets[curve].point7;
          break;
        
        case 7:
          value = config.offsets[curve].channel;
          break;
      }
    }
    
    // Move value box when point 1 is in the way
    if (((config.curve[curve].point1 < 50) && (type == CURVE)) || ((config.offsets[curve].point1 < 50) && (type == OFFSET)))
    {
      varBoxY = 0;
    }
    else
    {
      varBoxY = 40;      
    }

    // Move channel box when points 6 and 7 are in the way
    if (((config.curve[curve].point6 + config.curve[curve].point7) < 0) && (type == CURVE))
    {
      chanBoxY = 12;
    }
    else
    {
      chanBoxY = 51;
    }
    
    // Print the graph point values
    if (pointRef < 7)
    {
      itoa(value,pBuffer,10);
      uint8_t x = u8g2.getStrWidth(pBuffer);
      uint8_t y = 10;
      u8g2.setDrawColor(0);
      u8g2.drawBox(varBoxX, varBoxY + 2, (x + 5), (y + 2));    // Black box
      u8g2.setDrawColor(1);
      u8g2.drawFrame(varBoxX, varBoxY + 2, (x + 5), (y + 2));  // White outline
      u8g2.setCursor(varBoxX + 3, varBoxY + 3);
      u8g2.print(pBuffer);
    }
    
    // Print associated channel somewhere for the Generic curve
    if ((curve >= 4) && (type == CURVE))
    {
      uint8_t x = u8g2.getStrWidth((char*)(textMenu[reference + config.curve[curve].channel]));
      uint8_t y = 10;
      u8g2.setDrawColor(0);
      u8g2.drawBox((123 - x),(chanBoxY - y + 1), (x + 4), (y + 2));    // Black box
      u8g2.setDrawColor(1);
      u8g2.drawFrame((123 - x),(chanBoxY - y + 1), (x + 4), (y + 2));  // White outline
      printMenuText(0, 1, (reference + channel), (125 - x), (chanBoxY + 2 - y));  // Channel
    }

    u8g2.sendBuffer();

    // Slow the loop rate
    delay(100);   

    // Poll buttons when idle. This updates the button multiplier
    pollButtons(true);
  
    // Handle cursor Up/Down limits
    if (button == DOWN)
    {
      value = value - (range.increment * buttonMultiplier);
      
      // Limit values to set ranges
      if (value <= range.lower)
      {
        value = range.lower;
      }
    }

    if (button == UP)
    {
      value = value + (range.increment * buttonMultiplier);
      
      // Limit values to set ranges
      if (value >= range.upper)
      {
        value = range.upper;
      }
    }

    // Update values for next loop
    if (type == CURVE)
    {
      switch(pointRef)
      {
        case 0:
          config.curve[curve].point1 = value;
          break;
        
        case 1:
          config.curve[curve].point2 = value;
          break;
        
        case 2:
          config.curve[curve].point3 = value;
          break;
        
        case 3:
          config.curve[curve].point4 = value;
          break;
        
        case 4:
          config.curve[curve].point5 = value;
          break;
        
        case 5:
          config.curve[curve].point6 = value;
          break;
        
        case 6:
          config.curve[curve].point7 = value;
          break;
        
        case 7:
          config.curve[curve].channel = value;
          break;
      }
    }
    else
    {
      switch(pointRef)
      {
        case 0:
          config.offsets[curve].point1 = value;
          break;
        
        case 1:
          config.offsets[curve].point2 = value;
          break;
        
        case 2:
          config.offsets[curve].point3 = value;
          break;
        
        case 3:
          config.offsets[curve].point4 = value;
          break;
        
        case 4:
          config.offsets[curve].point5 = value;
          break;
        
        case 5:
          config.offsets[curve].point6 = value;
          break;
        
        case 6:
          config.offsets[curve].point7 = value;
          break;
        
        case 7:
          config.offsets[curve].channel = value;
          break;
      }
    }

    // Handle button 4
    if (button == ENTER)
    {
      // Cursor at far right
      if  (
          ((pointRef == (NUMBEROFPOINTS - 1)) && (curve < 4) && (type == CURVE)) ||
          ((pointRef == NUMBEROFPOINTS) && (curve >= 4) && (type == CURVE)) ||
          ((pointRef == (NUMBEROFPOINTS - 1)) && (type == OFFSET))
        )
      {
        button = ENTER;
      }
      // Move cursor right
      else
      {
        pointRef++;
        button = NONE;  
      }
    }

    // Handle button 1
    if (button == BACK)
    {
      // Cursor at far left
      if (pointRef == 0)
      {
        //button = ABORT;
        button = ENTER;
      }
      // Move cursor left
      else
      {
        pointRef--;
        button = NONE;
      }
    }
  } 
}

//************************************************************
// Update menu list, cursor, calculate selected item
// items  = Total number of menu items in list
// start  = Text list start position
// offset = Offset into special lists
// button = Current button state/value
// cursor*  = Location of cursor
// top*   = Item number currently on top line
// temp*  = Currently selected item number
//************************************************************

void updateMenu(uint16_t items, uint16_t start, uint16_t offset, uint8_t button, uint16_t* cursor, uint16_t* top, uint16_t* temp)
{
  // Temporarily add in offset :(
  *top = *top + offset;
  start = start + offset;

  // Calculate which function has been requested
  if (button == ENTER)
  {
    switch(*cursor) 
    {
      case LINE0:
        *temp = *top;
        break;
      case LINE1:
        *temp = *top + 1;
        break;  
      case LINE2:
        *temp = *top + 2;
        break;
      case LINE3:
        *temp = *top + 3;
        break;
      default:
        break;
    }
  }

  // Handle cursor Up/Down limits
  if (button == DOWN) 
  {
    switch(*cursor) 
    {
      case LINE0:
        if (items > 1) *cursor = LINE1;
        break;  
      case LINE1:
        if (items > 2) *cursor = LINE2;
        break;  
      case LINE2:
        if (items > 3) *cursor = LINE3;
        break;
      case LINE3:
        if (items > 4) *cursor = NEXTLINE;
        break;
      default:
        *cursor = NEXTLINE;
        break;
    }
  }

  if (button == UP) 
  {
    switch(*cursor) 
    {
      case LINE3:
        *cursor = LINE2;
        break;  
      case LINE2:
        *cursor = LINE1;
        break;
      case LINE1:
        *cursor = LINE0;
        break;
      case LINE0:
        *cursor = PREVLINE;
        break;
      default:
        *cursor = PREVLINE;
        break;
    }
  }

  if (button != NONE) 
  {
    // HJI menu_beep(1);
    delay(200);
  }

  // When cursor is at limits and button pressed
  if (*cursor == PREVLINE)                // Up       
  {
    *cursor  = LINE0;
    if (*top > start) 
    {
      *top = *top - 1;                // Shuffle list up
    }
  }
  
  if (*cursor == NEXTLINE)                // Down
  {
    *cursor  = LINE3;
    if ((*top + 3) < (uint16_t)((start + items) - 1)) // Compiler throws a warning here without the cast. top is uint16_t, start is uint8_t, items = uint8_t
    {
      *top = *top + 1;                // Shuffle list down
    }
  }

  // Remove temporary offset
  *top = *top - offset;
}

//************************************************************
// Special subroutine to print either numeric or text
// values = current value of item
// style = flag to indicate if value is numeric or a text link
// text_link = index of text to display
// x = horizontal location on screen
// y = vertical location on screen
//************************************************************

void printMenuText(int16_t values, uint8_t style, uint16_t textLink, uint8_t x, uint8_t y)
{
  if ((style == 0) || (style == 2) || (style == 3)) // Numeral
  {
    u8g2.setCursor(x, y);
    u8g2.print(values);
  }
  else
  {
    lcdDisplayText(textLink, x, y);
  }
}

//************************************************************
// Poll buttons, wait until something pressed, debounce and 
// return button info.
//************************************************************

uint8_t pollButtons(bool acceleration)
{
  static uint8_t buttonCount = 0;
  uint8_t buttons = 0;

  button = (pinb & 0xF0); // button is global, buttons is local

  while (button == NONE)          
  {
    buttons = (pinb & 0xF0);  
    delay(5);

    if (buttons != (pinb & 0xF0))
    {
      buttons = 0; // Buttons different
    }
    else // Buttons the same - update global
    {
      button = buttons;
    }

    // Reset button acceleration
    buttonCount = 0;
    buttonMultiplier = 1;
  }

  // Check for buttons being held down if requested
  if ((button != NONE) && (acceleration))
  {
    // Count the number of times incremented
    buttonCount++; 
    if (buttonCount >= 10)
    {
      buttonCount = 0;
      buttonMultiplier ++;
    }
  }

  return buttons;
}

void printCursor(uint8_t line)
{
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  u8g2.drawStr(CURSOROFFSET, line, "\x6F");  // Right
  u8g2.setFont(u8g2_font_helvR08_tr);
}
