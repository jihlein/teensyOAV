#include "Arduino.h"
#include "myPWMServo.h"

/*
  myPWMServo.cpp - Hardware Servo Timer Library
  http://arduiniana.org/libraries/myPWMServo/
  Author: Jim Studt, jim@federated.com
  Copyright (c) 2007 David A. Mellis.  All right reserved.
  renamed to myPWMServo by Mikal Hart
  ported to other chips by Paul Stoffregen
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define NO_PULSE (0)

uint32_t myPWMServo::attachedpins[(NUM_DIGITAL_PINS + 31) / 32]; // 1 bit per digital pin

myPWMServo::myPWMServo() : pin(255), pulseWidth(NO_PULSE) {}

uint8_t myPWMServo::attach(uint8_t pinArg, uint16_t frequency, uint16_t min, uint16_t max)
{
	if (pinArg < 0 || pinArg >= NUM_DIGITAL_PINS) return 0;
	if (!digitalPinHasPWM(pinArg)) return 0;
	pin = pinArg;
	pulseWidthMin = min;
	pulseWidthMax = max;
	analogWriteFrequency(pin, frequency);
	pulseWidthMicroSeconds = 1e6 / frequency; 
	pulseWidth = 0;
	digitalWrite(pin, LOW);
	pinMode(pin, OUTPUT);
	attachedpins[pin >> 5] |= (1 << (pin & 31));
	return 1;
}

void myPWMServo::write(uint16_t pulseWidthArg)
{
	if (pin >= NUM_DIGITAL_PINS) return;
	if (pulseWidthArg < pulseWidthMin) pulseWidthArg = pulseWidthMin;
	if (pulseWidthArg > pulseWidthMax) pulseWidthArg = pulseWidthMax;
	pulseWidth = pulseWidthArg;
    uint32_t duty = pulseWidth * 4096 / pulseWidthMicroSeconds;
#if TEENSYDUINO >= 137
	noInterrupts();
	uint32_t oldres = analogWriteResolution(12);
	analogWrite(pin, duty);
	analogWriteResolution(oldres);
	interrupts();
#else
	analogWriteResolution(12);
	analogWrite(pin, duty);
#endif
}

uint8_t myPWMServo::attached()
{
	if (pin >= NUM_DIGITAL_PINS) return 0;
	return (attachedpins[pin >> 5] & (1 << (pin & 31))) ? 1 : 0;
}
