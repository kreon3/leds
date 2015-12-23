/*
  Button

 Turns on and off a light emitting diode(LED) connected to digital
 pin 13, when pressing a pushbutton attached to pin 2.


 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached to pin 2 from +5V
 * 10K resistor attached to pin 2 from ground

 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.


 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Button
 */

// constants won't change. They're used here to
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
const int BUTTON_1_PIN = 7;
const int BUTTON_2_PIN = 8;
const int BUTTON_3_PIN = 9;
const int BUTTON_4_PIN = 10;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  
  Serial.begin(9600);
  Serial.println("Button Test");
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);
  pinMode(BUTTON_4_PIN, INPUT);
}

void PrintState(int pin_state) {
  if (pin_state == HIGH) {
    Serial.print("HIGH ");
  } else {
    Serial.print("LOW ");
  }
}

void loop() {
  int b1pin = digitalRead(BUTTON_1_PIN);
  int b2pin = digitalRead(BUTTON_2_PIN);
  int b3pin = digitalRead(BUTTON_3_PIN);
  int b4pin = digitalRead(BUTTON_4_PIN);
  Serial.print("Buttons state ");
  PrintState(b1pin);
  PrintState(b2pin);
  PrintState(b3pin);
  PrintState(b4pin);
  Serial.println("");
  bool b1state = b1pin == HIGH ? true : false;
  bool b2state = b2pin == HIGH ? true : false;
  bool b3state = b3pin == HIGH ? true : false;
  bool b4state = b4pin == HIGH ? true : false;

  bool bState = b1state ^ b2state ^ b3state ^ b4state;
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (bState) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}
