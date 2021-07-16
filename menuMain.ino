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

#ifdef ERROR_LOG
#define MAINITEMS 24  // Number of menu items
#else
#define MAINITEMS 23  // Number of menu items
#endif

#define MAINSTART 77

//************************************************************
// Main menu-specific setup
//************************************************************

uint16_t mainTop = MAINSTART;

void menuMain(void)
{
  static uint16_t mainCursor = LINE0;
  static uint16_t mainTemp = 0;
  static uint8_t oldMenu = 0;

  button = NONE;

  // HJI menu_beep(1);

  // Wait until user's finger is off button 1
  waitButton1();
  
  while(button != BACK)
  {
    u8g2.clearBuffer(); 

    // Print menu
    printMenuFrame(BASIC);
    
    for (uint16_t i = 0; i < 4; i++)
      lcdDisplayText(mainTop + i, ITEMOFFSET,(uint8_t)pgm_read_byte(&lines[i]));
    
    printCursor(mainCursor);
    u8g2.sendBuffer();

    // Poll buttons when idle
    pollButtons(true);

    // Handle menu changes
    updateMenu(MAINITEMS, MAINSTART, 0, button, &mainCursor, &mainTop, &mainTemp);

    // If main menu item has changed, reset sub-menu positions
    // and flag to sub-menus that positions need to be reset
    if (mainTemp != oldMenu)
    {
      cursor = LINE0;
      menuTemp = 0;
      oldMenu = mainTemp;
      menuFlag = 1;
    }

    // If ENTER pressed, jump to menu 
    if (button == ENTER)
    {
      doMainMenuItem(mainTemp);
      button = NONE;

      // Wait until user's finger is off button 1
      waitButton1();
    }
  }
}

void doMainMenuItem(uint8_t menuItem)
{
  switch(menuItem) 
  {
    case MAINSTART:
      menuRcSetup(2);     // 1.General
      break;
    case MAINSTART+1:
       menuRcSetup(1);    // 2.RX setup
      break;
    case MAINSTART+2:
      displayRCInput();   // 3.RX inputs
      break;
    case MAINSTART+3:
      displaySticks();    // 4.Stick polarity
      break;
    case MAINSTART+4:
      displaySensors();   // 5.Sensor calibration
      break;
    case MAINSTART+5:
      displayBalance();   // 6.Level meter
      break;
    case MAINSTART+6:
      menuFlight(0);      // 7.Flight profile 1
      break;
    case MAINSTART+7:
      menuFlight(1);      // 8.Flight profile 2
      break;
    case MAINSTART+8:
      menuCurves();       // 9.Curves menu
      break;
    case MAINSTART+9:
      menuOffsets();      // 10.Output offsets
      break;
    case MAINSTART+10:
      menuMixer(0);       // 11.OUT1 Mixer
      break;
    case MAINSTART+11:
      menuMixer(1);       // 12.OUT2 Mixer
      break;
    case MAINSTART+12:
      menuMixer(2);       // 13.OUT3 Mixer
      break;
    case MAINSTART+13:
      menuMixer(3);       // 14.OUT4 Mixer
      break;
    case MAINSTART+14:
      menuMixer(4);       // 15.OUT5 Mixer
      break;
    case MAINSTART+15:
      menuMixer(5);       // 16.OUT6 Mixer
      break;
    case MAINSTART+16:
      menuMixer(6);       // 17.OUT7 Mixer
      break;
    case MAINSTART+17:
      menuMixer(7);       // 18.OUT8 Mixer
      break;
    case MAINSTART+18:
      menuServoSetup(1);  // 19.Servo direction
      break;
    case MAINSTART+19:
      menuServoSetup(2);  // 20.Neg. Servo trvl. (%)
      break;
    case MAINSTART+20:
      menuServoSetup(3);  // 21.Pos. Servo trvl. (%)
      break;
    case MAINSTART+21:
      menuChannel();      // 22.Custom Ch. order
      break;
    case MAINSTART+22:
      displayInOut();     // 23.IO menu
      break;
    case MAINSTART+23:
      menuLog();          // 24.Error log
      break;
    default:
      break;  
  }
}

void waitButton1(void)
{
  while(digitalRead(BUTTON1) == 0)
  {
    delay(50);
  }
}

void waitButton4(void)
{
  while(digitalRead(BUTTON4) == 0)
  {
    delay(50);
  }
}
