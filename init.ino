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
// * Code
// ************************************************************

void init(void)
{
  uint8_t i;
  
  //***********************************************************
  // Start up
  //***********************************************************

  // Load EEPROM settings
  updated = initialEEPROMConfigLoad(); // Config now contains valid values

  //***********************************************************
  // I/O setup
  //***********************************************************
  
  // Button Inputs
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  // Heartbeat LED
  pinMode(HEARTBEAT_LED, OUTPUT);
  
  // Status LED
  pinMode(STATUS_LED, OUTPUT);
  
  // PWM Outputs
  setupServos();
   
  //***********************************************************
  // RX Configuration
  //***********************************************************

  if (config.rxMode == SBUS)
  {
    sbusRx.Begin();
  }
  else if (config.rxMode == SPEKTRUM)
  {
    Serial3.begin(115000);
  }

  //***********************************************************
  // LCD initialization
  //***********************************************************

  u8g2.begin();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_helvR08_tr);
  
  //***********************************************************
  // ESC calibration
  //***********************************************************
  
  // Calibrate ESCs if ONLY buttons 1 and 4 pressed
  if ((digitalRead(BUTTON1) == 0) && (digitalRead(BUTTON4) == 0))
  {
    // Display calibrating message
    u8g2.clearBuffer();
    lcdDisplayText(59, 17, 25);
    u8g2.sendBuffer();
        
    // Set all PWM outputs to 50 Hz
    output0.attach( 0, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output1.attach( 1, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output2.attach( 2, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output3.attach( 4, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output4.attach( 5, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output5.attach( 6, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output6.attach( 7, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output7.attach(14, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output8.attach(24, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
    output9.attach(28, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  
    // For each output
    for (i = 0; i < MAX_OUTPUTS; i++)
    {
      // Check for motor marker
      if (config.channel[i].motorMarker == MOTOR)
      {
        // Set output to maximum pulse width
        servoCmd[i] = MOTOR_100;
      }
      else
      {
        servoCmd[i] = SERVO_CENTER;
      }
    }
          
    // Output HIGH pulse (1.9ms) until buttons released
    while ((digitalRead(BUTTON1) == 0) && (digitalRead(BUTTON4) == 0))
    {
      output0.write(servoCmd[0]);
      output1.write(servoCmd[1]);
      output2.write(servoCmd[2]);
      output3.write(servoCmd[3]);
      output4.write(servoCmd[4]);
      output5.write(servoCmd[5]);
      output6.write(servoCmd[6]);
      output7.write(servoCmd[7]);
      output8.write(servoCmd[8]);
      output9.write(servoCmd[9]);

      // Loop rate = 20ms (50Hz)
      delay(20);      
    }

    // Output LOW pulse (1.1ms) after buttons released
    // For each output
    for (i = 0; i < MAX_OUTPUTS; i++)
    {
      // Check for motor marker
      if (config.channel[i].motorMarker == MOTOR)
      {
        // Set output to minimum pulse width
        servoCmd[i] = MOTOR_0;
      }
    }   

    // Loop forever here
    while(1)
    {
      output0.write(servoCmd[0]);
      output1.write(servoCmd[1]);
      output2.write(servoCmd[2]);
      output3.write(servoCmd[3]);
      output4.write(servoCmd[4]);
      output5.write(servoCmd[5]);
      output6.write(servoCmd[6]);
      output7.write(servoCmd[7]);
      output8.write(servoCmd[8]);
      output9.write(servoCmd[9]);

      // Loop rate = 20ms (50Hz)
      delay(20);      
    }
  }

  //***********************************************************
  // Reset EEPROM settings
  //***********************************************************

  // Reload default eeprom settings if middle two buttons are pressed
  if ((digitalRead(BUTTON2) == 0) && (digitalRead(BUTTON3) == 0))
  {
    // Display reset message
    u8g2.clearBuffer();
    lcdDisplayText(262, 47, 25);  // "Reset"
    u8g2.sendBuffer();
    
    // Reset EEPROM settings
    setEEPROMDefaultConfig();

    // Save settings
    saveConfigToEEPROM();

    delay(2000);  // Save is now too fast to show the "Reset" text long enough
  }

  // Display message in place of logo when updating eeprom structure
  u8g2.clearBuffer();
    
  if (updated)
  {
    lcdDisplayText(259, 37, 13); // "UPDATING"
    lcdDisplayText(260, 39, 37); // "SETTINGS"
  }
  else
  {
    u8g2.drawStr(10,  0, "teensy");
    u8g2.setFont(u8g2_font_fub30_tr);
    u8g2.drawStr(20, 17, "OAV");
    u8g2.setFont(u8g2_font_helvR08_tr);    
  }

  u8g2.sendBuffer();
  delay(4000);
    
  //***********************************************************
  // i2c init
  //*********************************************************** 

  Wire.begin();
  Wire.setClock(800000);
  
  mpu.initialize();
  mpu.setClockSource(MPU6050_CLOCK_PLL_XGYRO);
  mpu.setInterruptDrive(MPU6050_INTCFG_INT_OPEN_BIT);
  mpu.setDLPFMode(6 - config.mpu6050LPF);
  mpu.setRate(1);  // 500 Hz
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);

  //***********************************************************
  // Remaining init tasks
  //***********************************************************

  // Display "Hold steady" message
  u8g2.clearBuffer();
  lcdDisplayText(263, 27, 25);  // "HOLD STEADY"
  u8g2.sendBuffer();
  
  // Do startup tasks
  analogReadRes(12);
  analogReadAveraging(4);
  
  // Initial gyro calibration
  delay(2000);
  
  if (!calibrateGyrosSlow())
  {
    u8g2.clearBuffer();
    lcdDisplayText(61, 34, 25); // "CAL FAILED"
    u8g2.sendBuffer();
    delay(1000);
      
    SCB_AIRCR = 0x05FA0004;
    
    while(1);  // Wait for reboot
  }

  // Update voltage detection
  // HJI SystemVoltage = GetVbat();        // Check power-up battery voltage
  
  updateLimits();  // Update travel and trigger limits

  // Disarm on start-up if Armed setting is ARMABLE
  if (config.armMode == ARMABLE)
  {
    generalError |= (1 << DISARMED);  // Set disarmed bit
  }

  // Reset IMU
  resetIMU();

  // Beep that init is complete
  // Check buzzer mode first
  if (config.buzzer == ON)
  {
    // HJI LVA = 1;
    delay(25);
    // HJI LVA = 0;
  }

#ifdef ERROR_LOG
// If restart, log it as such
add_log(REBOOT);
#endif

}
