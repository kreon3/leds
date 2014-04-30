/*
* _5050_leds.cpp
*
* Created: 3/3/2014 7:55:46 PM
*  Author: Dave
*/


#include <avr/io.h>
#include <avr/interrupt.h>


#line 1 "_5050leds.ino"
// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#define REDPIN 2
#define GREENPIN 4
#define BLUEPIN 6

#define FADESPEED 250     // make this higher to slow down

#include "Arduino.h"
void setup();
void basicRotate();
void zeroPins();
void increasePin(int pin);
void decreasePin(int pin);
void loop();
#line 11
void setup() {
	pinMode(REDPIN, OUTPUT);
	pinMode(GREENPIN, OUTPUT);
	pinMode(BLUEPIN, OUTPUT);
	pinMode(13, OUTPUT);
	zeroPins();
}

void basicRotate() {
	
	int r, g, b;
	
	// fade from blue to violet
	for (r = 0; r < 256; r++) {
		analogWrite(REDPIN, r);
		delay(FADESPEED);
	}
	// fade from violet to red
	for (b = 255; b > 0; b--) {
		analogWrite(BLUEPIN, b);
		delay(FADESPEED);
	}
	// fade from red to yellow
	for (g = 0; g < 256; g++) {
		analogWrite(GREENPIN, g);
		delay(FADESPEED);
	}
	// fade from yellow to green
	for (r = 255; r > 0; r--) {
		analogWrite(REDPIN, r);
		delay(FADESPEED);
	}
	// fade from green to teal
	for (b = 0; b < 256; b++) {
		analogWrite(BLUEPIN, b);
		delay(FADESPEED);
	}
	// fade from teal to blue
	for (g = 255; g > 0; g--) {
		analogWrite(GREENPIN, g);
		delay(FADESPEED);
	}
}

void colorRGB(uint8_t r, uint8_t g, uint8_t b) {
	analogWrite(REDPIN, r);
	analogWrite(GREENPIN, g);
	analogWrite(BLUEPIN, b);
}

void colorWheel(uint8_t wheel_pos) {
	uint8_t inc = wheel_pos % 85 * 3;
	uint8_t decr = 255 - inc;
	if (wheel_pos < 85) {
		colorRGB(inc, decr, 0);
		} else if (wheel_pos < 170) {
		colorRGB(decr, 0, inc);
		} else {
		colorRGB(0, inc, decr);
	}
}

void wheelRotate() {
	for (uint8_t i = 0; i < 255; ++i) {
		colorWheel(i);
		delay(FADESPEED);
	}
}

void zeroPins() {
	analogWrite(REDPIN, 0);
	analogWrite(GREENPIN, 0);
	analogWrite(BLUEPIN, 0);
}

void increasePin(int pin) {
	for (int i = 0; i < 256; ++i) {
		analogWrite(pin, i);
		delay(FADESPEED);
	}
}

void decreasePin(int pin) {
	for (int i = 255; i >= 0 ; --i) {
		analogWrite(pin, i);
		delay(FADESPEED);
	}
}

void loop() {
	/*
	zeroPins();
	delay(1000);
	increasePin(REDPIN);
	decreasePin(REDPIN);
	
	
	zeroPins();
	delay(1000);
	increasePin(GREENPIN);
	decreasePin(GREENPIN);
	
	zeroPins();
	delay(1000);
	
	increasePin(BLUEPIN);
	decreasePin(BLUEPIN);
	delay(2000);*/
	
	//basicRotate();
	wheelRotate();
}

/*
int main(void)
{
while(1)
{
digitalWrite(13, HIGH);
delay(400);
digitalWrite(13, LOW);
delay(200);
//loop();
}
}*/