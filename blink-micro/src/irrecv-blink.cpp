// Test recieving IR data to change the blink rate of the LED.

#include <Arduino.h>
#include <IRremote.h>

namespace irrecv_blink {

// Note not having the pin connected can cause the controller to fail to
// initialize.
// Pins that work: 3, 4, 5, 6, 7, 8, 9, 10, 11, 12

// Pins that don't work: 2

int RECV_PIN = 12;
#define LED_PIN 13

IRrecv irrecv(RECV_PIN);

decode_results results;

int32_t on_delay_ms = 300;
int32_t off_delay_ms = 300;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(LED_PIN, OUTPUT);     // set pin as output
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFFFFFFF) {
      // Continuation value.
      Serial.print("+");
    } else {
      Serial.println("");
      Serial.print(results.decode_type);
      Serial.print(" ");
      Serial.print(0xFFFF & results.value, HEX);

      on_delay_ms = ((0xFF00 & results.value) >> 8) * 3;
      off_delay_ms = (0xFF & results.value) * 3;

    }
    irrecv.resume(); // Receive the next value
  }

  digitalWrite(LED_PIN, HIGH);
  delay(on_delay_ms);
  digitalWrite(LED_PIN, LOW);
  delay(off_delay_ms);
}

}  // namespace irrecv_blink

// void setup() { irrecv_blink::setup();}
// void loop() { irrecv_blink::loop();}
