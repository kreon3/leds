#include <Adafruit_NeoPixel.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(68, PIN, NEO_GRB + NEO_KHZ800);

/* TODO: The basic definition for pattern libraries:
 * Key elements of functions:
 *   Colors
 *   Directions
 *   Speed/delay
 *   Duration (for repeating cycles).
 *
 * Ambivalent parameters can be explicit or random (if no input).
 * 
 * Major Axis:
 *   Color
 *   Brightness
 *   Pulse rate
 *   Movement
 *
 * Patterns to write:
 *   By rotation change color, buy acceleration change brightness.
 * 
 * Color class used for specifying colors.
 */
 
int led = 13;
void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  
  pinMode(led, OUTPUT);     
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  //rainbow(20);
  //rainbowCycle(20);
//  
//  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);               // wait for a second
//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);               // wait for a second

  randomColorsAndBrightness(1000, 30);
  sparkle();
}

void colorAll(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void colorAllRandMax(uint32_t c, uint8_t max_brightness) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c * random(max_brightness) / 255);
  }
  strip.show();
}

// TODO: Random color should allow a factor of multi-color aversion. If all 
// colors are random, there is a propensity to converge to white.

uint32_t randomColor() {
  return strip.Color(random(255), random(255), random(255));
}

void randomColorsAndBrightness(uint32_t duration_ms, uint32_t frequency_ms) {
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    
    colorAllRandMax(randomColor(), 100);
  }
  strip.show();
}

/*
uint32_t randomColor(uint_8 max_brightness) {
  return strip.Color(random(max_brightness), random(max_brightness), random(max_brightness));
}*/

void randomColors(uint32_t duration_ms, uint32_t frequency_ms) {
  for (int i = 0; i < duration_ms; i += frequency_ms) {
    colorAll(randomColor());
    delay(frequency_ms);
  }
}
/*
void randomColors(uint32_t duration_ms, uint32_t frequency_ms) {
  for (int i = 0; i < duration_ms; i += frequency_ms) {
    colorAllRandomBrightness(randomColor(), 100);
    delay(frequency_ms);
  }
}*/

#define LIT 40

void sparkle() {
  colorAll(0);
  randomSeed(42);
  uint8_t maxSteps = 100;
  // Each item increases to a target color with a rate.
  uint32_t targetColors[LIT];
  uint32_t currentColors[LIT];
  uint32_t colorStep[LIT];
  uint16_t ledIndex[LIT];
  int8_t ledUp[LIT];  // Positive for up, negative for down, 0 for unset.
  uint8_t rBias, bBias, gBias;
    
  
  // Not sure if this is required, but makew sure the indexes are empty.
  for (uint16_t i = 0; i < LIT; ++i) {
    ledUp[i] = 0;
  }  
  
  
  
  for (uint32_t cycle = 0; cycle < 30000; ++cycle) {
  /*  rBias = min(0, max(100, rBias + random(-2, 2)));
    bBias = min(0, max(100, bBias + random(-2, 2)));
    gBias = min(0, max(100, gBias + random(-2, 2)));*/
    if (cycle % 300 == 0) {
      rBias = random(0, 80);
      bBias = random(0, 80);
      gBias = random(0, 80);
    }
    
    for (uint16_t i = 0; i < LIT; ++i) {
      if (ledUp[i] == 0) {
        // Reinit the state;
        boolean withBias = random(0, 100) > 3 ? true : false;
        uint8_t r = withBias ? rBias + random(10) : random(100);
        uint8_t g = withBias ? gBias + random(10) : random(100);
        uint8_t b = withBias ? bBias + random(10) : random(100);
        uint8_t steps = random(10, maxSteps);
        ledUp[i] = 1;
        targetColors[i] = strip.Color(r, g, b);
        colorStep[i] = strip.Color(max(1, r / steps), max(1, g / steps), max(1, b / steps));
        currentColors[i] = strip.Color(0, 0, 0);
        ledIndex[i] = random(strip.numPixels());
      } 
      
      if (ledUp[i] > 0) {
        currentColors[i] += colorStep[i];
      } else {
        currentColors[i] -= colorStep[i];
      }
      
      strip.setPixelColor(ledIndex[i], currentColors[i]);
      
      if (currentColors[i] > targetColors[i]) {
        ledUp[i] = -1;
      }
      
      if (currentColors[i] < colorStep[i]) {
        // Reset the index to restart the sequence.
        strip.setPixelColor(ledIndex[i], 0);
        ledUp[i] = 0;
        ledIndex[i] = 0;
      }
      
    }
    strip.show();
    delay(33);
  }
  
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

