// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 2
#define GREENPIN 3
#define BLUEPIN 4
 
#define FADESPEED 5     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
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
  for (int i = 255; i > 0 ; --i) {
    analogWrite(pin, i);
    delay(FADESPEED);
  }
}

void loop() {
    zeroPins();
//    analogWrite(BLUEPIN, 150);
    
//    analogWrite(REDPIN, 255);
//    increasePin(BLUEPIN);
//    decreasePin(BLUEPIN);
  zeroPins();
  increasePin(REDPIN);
  decreasePin(REDPIN);
  
  zeroPins();
  increasePin(GREENPIN);
  decreasePin(GREENPIN);

  zeroPins();
  increasePin(BLUEPIN);
  decreasePin(BLUEPIN);


}
