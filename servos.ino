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

void servoCmds(void)
{
  int32_t temp;
  uint8_t i = 0;

#if 0
  Serial.print(servoOut[0]); Serial.print("\t");
  Serial.print(servoOut[1]); Serial.print("\t");
  Serial.print(servoOut[2]); Serial.print("\t");
  Serial.print(servoOut[3]); Serial.print("\t");
  Serial.print(servoOut[4]); Serial.print("\t");
  Serial.print(servoOut[5]); Serial.print("\t");
  Serial.print(servoOut[6]); Serial.print("\t");
  Serial.print(servoOut[7]); Serial.print("\t");
  Serial.print(servoOut[8]); Serial.print("\t");
  Serial.println(servoOut[9]);
#endif

  // Re-span numbers from internal values to microseconds and check limits
  for (i = 0; i < MAX_OUTPUTS; i++)
  {
    temp = servoOut[i];  // Promote to 32 bits

    // Check for motor marker and ignore if set
    if (config.channel[i].motorMarker != MOTOR)
    {
      // Scale servo from 2500~5000 to 875~2125
      temp = ((temp - 3750) >> 1) + 1500;
    }
    else
    {
      // Scale motor from 2500~5000 to 1000~2000
      temp = ((temp << 2) + 5) / 10;  // Round and convert  
    }
    
    // Enforce min, max travel limits in microseconds
    if (temp > config.limits[i].maximum)
    {
      temp = config.limits[i].maximum;
    }

    else if (temp < config.limits[i].minimum)
    {
      temp = config.limits[i].minimum;
    }

    servoCmd[i] = (uint16_t)temp;
  }

  // Check for motor flags if throttle is below arming minimum or disarmed
  // and set all motors to minimum throttle if so.
  if  ((monopolarThrottle < THROTTLEIDLE) || (generalError & (1 << DISARMED)))
  {
    // For each output
    for (i = 0; i < MAX_OUTPUTS; i++)
    {
      // Check for motor marker
      if (config.channel[i].motorMarker == MOTOR)
      {
        // Set output to minimum pulse width (1000us)
        servoCmd[i] = MOTORMIN;
      }
    }
  }

#if 0
  Serial.print(servoCmd[0]); Serial.print("\t");
  Serial.print(servoCmd[1]); Serial.print("\t");
  Serial.print(servoCmd[2]); Serial.print("\t");
  Serial.print(servoCmd[3]); Serial.print("\t");
  Serial.print(servoCmd[4]); Serial.print("\t");
  Serial.print(servoCmd[5]); Serial.print("\t");
  Serial.print(servoCmd[6]); Serial.print("\t");
  Serial.print(servoCmd[7]); Serial.print("\t");
  Serial.print(servoCmd[8]); Serial.print("\t");
  Serial.println(servoCmd[9]);
#endif

}

void setupServos(void)
{
  // PWM Pin 0
  if      (config.channel[0].motorMarker == MOTOR)  output0.attach( 0, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[0].motorMarker == ASERVO) output0.attach( 0, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output0.attach( 0, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 1
  if      (config.channel[1].motorMarker == MOTOR)  output1.attach( 1, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[1].motorMarker == ASERVO) output1.attach( 1, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output1.attach( 1, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 2
  if      (config.channel[2].motorMarker == MOTOR)  output2.attach( 2, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[2].motorMarker == ASERVO) output2.attach( 2, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output2.attach( 2, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 4
  if      (config.channel[3].motorMarker == MOTOR)  output3.attach( 4, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[3].motorMarker == ASERVO) output3.attach( 4, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output3.attach( 4, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 5
  if      (config.channel[4].motorMarker == MOTOR)  output4.attach( 5, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[4].motorMarker == ASERVO) output4.attach( 5, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output4.attach( 5, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 6
  if      (config.channel[5].motorMarker == MOTOR)  output5.attach( 6, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[5].motorMarker == ASERVO) output5.attach( 6, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output5.attach( 6, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 7
  if      (config.channel[6].motorMarker == MOTOR)  output6.attach( 7, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[6].motorMarker == ASERVO) output6.attach( 7, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output6.attach( 7, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 14
  if      (config.channel[7].motorMarker == MOTOR)  output7.attach(14, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[7].motorMarker == ASERVO) output7.attach(14, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output7.attach(14, DIGITALFREQ, DIGITALMIN, DIGITALMAX);

  // PWM Pin 24
  if      (config.channel[8].motorMarker == MOTOR)  output8.attach(24, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[8].motorMarker == ASERVO) output8.attach(24, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output8.attach(24, DIGITALFREQ, DIGITALMIN, DIGITALMAX);
  // PWM Pin 28
  if      (config.channel[9].motorMarker == MOTOR)  output9.attach(28, ONESHOTFREQ, ONESHOTMIN, ONESHOTMAX);
  else if (config.channel[9].motorMarker == ASERVO) output9.attach(28, ANALOGFREQ,  ANALOGMIN,  ANALOGMAX);
  else                                              output9.attach(28, DIGITALFREQ, DIGITALMIN, DIGITALMAX);
}

void send500HzServos(void)
{
  if (config.channel[0].motorMarker == MOTOR)  output0.write((servoCmd[0] + 4) / 8);
  if (config.channel[1].motorMarker == MOTOR)  output1.write((servoCmd[1] + 4) / 8);
  if (config.channel[2].motorMarker == MOTOR)  output2.write((servoCmd[2] + 4) / 8);
  if (config.channel[3].motorMarker == MOTOR)  output3.write((servoCmd[3] + 4) / 8);
  if (config.channel[4].motorMarker == MOTOR)  output4.write((servoCmd[4] + 4) / 8);
  if (config.channel[5].motorMarker == MOTOR)  output5.write((servoCmd[5] + 4) / 8);
  if (config.channel[6].motorMarker == MOTOR)  output6.write((servoCmd[6] + 4) / 8);
  if (config.channel[7].motorMarker == MOTOR)  output7.write((servoCmd[7] + 4) / 8);
  if (config.channel[8].motorMarker == MOTOR)  output8.write((servoCmd[8] + 4) / 8);
  if (config.channel[9].motorMarker == MOTOR)  output9.write((servoCmd[9] + 4) / 8);
}

void send250HzServos(void)
{
  if (config.channel[0].motorMarker == DSERVO)  output0.write(servoCmd[0]);
  if (config.channel[1].motorMarker == DSERVO)  output1.write(servoCmd[1]);
  if (config.channel[2].motorMarker == DSERVO)  output2.write(servoCmd[2]);
  if (config.channel[3].motorMarker == DSERVO)  output3.write(servoCmd[3]);
  if (config.channel[4].motorMarker == DSERVO)  output4.write(servoCmd[4]);
  if (config.channel[5].motorMarker == DSERVO)  output5.write(servoCmd[5]);
  if (config.channel[6].motorMarker == DSERVO)  output6.write(servoCmd[6]);
  if (config.channel[7].motorMarker == DSERVO)  output7.write(servoCmd[7]);
  if (config.channel[8].motorMarker == DSERVO)  output8.write(servoCmd[8]);
  if (config.channel[9].motorMarker == DSERVO)  output9.write(servoCmd[9]);
}

void send50HzServos(void)
{
  if (config.channel[0].motorMarker == ASERVO)  output0.write(servoCmd[0]);
  if (config.channel[1].motorMarker == ASERVO)  output1.write(servoCmd[1]);
  if (config.channel[2].motorMarker == ASERVO)  output2.write(servoCmd[2]);
  if (config.channel[3].motorMarker == ASERVO)  output3.write(servoCmd[3]);
  if (config.channel[4].motorMarker == ASERVO)  output4.write(servoCmd[4]);
  if (config.channel[5].motorMarker == ASERVO)  output5.write(servoCmd[5]);
  if (config.channel[6].motorMarker == ASERVO)  output6.write(servoCmd[6]);
  if (config.channel[7].motorMarker == ASERVO)  output7.write(servoCmd[7]);
  if (config.channel[8].motorMarker == ASERVO)  output8.write(servoCmd[8]);
  if (config.channel[9].motorMarker == ASERVO)  output9.write(servoCmd[9]);
}
