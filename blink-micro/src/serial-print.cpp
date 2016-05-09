// Basic program to test monitoring the serial port
#include "Arduino.h"
namespace serial {

int count_this;
void setup(void) {
  Serial.begin(9600);
  Serial.println("Reset!");
  count_this = 0;
}
void loop() {

  Serial.print("Loop: ");
  Serial.println(count_this);
  delay(1000);
  ++count_this;
}

}  // namespace serial

// void setup() { serial::setup(); }
// void loop() { serial::loop(); }
