#include "Arduino.h"

/*
  Button

 Turns on and off a light emitting diode(LED) connected to digital
 pin 13, when pressing a pushbutton attached to pin 2 xor 3.


 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached to pin 2 from +5V
 * pushbutton attached to pin 3 from +5V
 * 10K resistor attached to pin 2 and 3 from ground

 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
*/

namespace two_buttons {

// constants won't change. They're used here to
// set pin numbers:
#define BUTTON_PIN_A 2
#define BUTTON_PIN_B 3
const int ledPin =  13;      // the number of the LED pin

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN_A, INPUT);
  pinMode(BUTTON_PIN_B, INPUT);
}

void loop() {

  // digitalWrite(ledPin, HIGH);
  // delay(5);
  // read the state of the pushbutton value:
  if ((digitalRead(BUTTON_PIN_A) == HIGH) ^ (digitalRead(BUTTON_PIN_B) == HIGH)) {
      // turn LED on:
      digitalWrite(ledPin, HIGH);
  } else {
      // turn LED off:
      digitalWrite(ledPin, LOW);
  }
  // delay(1000);
}

}  // namespace two_buttons

// Uncomment to run this as the arduino main.
// void setup() { two_buttons::setup(); }
// void loop() { two_buttons::loop(); }
