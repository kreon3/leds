/* Code to perform varios LED configurations. */

#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>



enum Pins {
  SDA_PIN = 2,
  SCA_PIN = 3,

  LED_PIN = 6,

  KEYPAD_COL_1_PIN = 8,
  KEYPAD_COL_2_PIN = 7,
  KEYPAD_COL_3_PIN = 10,
  KEYPAD_COL_4_PIN = 9,
  KEYPAD_ROW_PIN = 11
};

///////////////////////////////////////////////
/// Forward declares.
void beginPattern();
void returnToDisplay();



///////////////////////////////////////////////
/// Globals used to set state and the like.

long time_ = 0;

enum Pattern {
  SPARKLE_DIM = 0,
  SPARKLE_BRIGHT = 1,
  SPARKLE_ACCEL = 2,
  OFF = 3
};
enum Pattern pattern_ = SPARKLE_DIM;

uint8_t brightness_ = 5;  // Brightness of the strips.

// Accelerometer readings.a
float linear_acceleration_magnitude_ = 0;
sensors_vec_t orientation_;  // key items are floats for roll, pitch, and heading.

void initState() {
  pattern_ = SPARKLE_DIM;
  brightness_ = 5;
  linear_acceleration_magnitude_ = 0;
}

///////////////////////////////////////////////
/// State modifiers.
void increaseBrightness() {
  if (brightness_ < 10) {
    brightness_++;
  }
}
void decreaseBrightness() {
  if (brightness_ > 0) {
    brightness_--;
  }
}

///////////////////////////////////////////////
/// Initialization code.

// Keypad stuff.
const byte ROWS = 1; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '4'}
};
byte rowPins[ROWS] = {KEYPAD_ROW_PIN}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  KEYPAD_COL_1_PIN,
  KEYPAD_COL_2_PIN,
  KEYPAD_COL_3_PIN,
  KEYPAD_COL_4_PIN
}; //connect to the column pinouts of the keypad
Keypad keypad_ = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);  // The 1x4 keypad.
void initKeypad() {
  // Done using the global constructor.
}

// Accelerometer stuff.
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno;
void initAccelerometer() {
  bno = Adafruit_BNO055(55);
  /* Initialise the sensor */
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  }
  bno.setExtCrystalUse(true);
}

Adafruit_NeoPixel strip_ = Adafruit_NeoPixel(30, LED_PIN, NEO_GRB + NEO_KHZ800);
void initLeds() {
  strip_.begin();
}

void setup() {
  Serial.begin(9600);
  initState();
  Serial.println("Beginning setup");
  Serial.println("Init LEDs");
  initLeds();
  Serial.println("Init Keypad");
  initKeypad();
  Serial.println("Init Accelerometer");
  initAccelerometer();
  Serial.println("Setup complete");

  for (uint8_t i; i < 100; ++i) {
    uint8_t val = i <= 50 ? i : 100-i;
    setLedAll(color(val, val, val));
    strip_.show();
    
    delay(20);
  }
  
  for (uint8_t i; i < 30; ++i) {
    setLedAll(color(0, 0, 0));
    setLed(i % 10, i / 10, color(100, 100, 100));
    strip_.show();
    delay(70);
  }
  
  beginPattern();
}



////////////////////////////////////////////////
// Button processing.


void handleButton(int button_index) {
  Serial.print("Handle Button ");
  Serial.println(button_index);
  switch (button_index) {
    case 0: pattern_ = SPARKLE_DIM; break;
    case 1: pattern_ = SPARKLE_BRIGHT; break;
    case 2: pattern_ = SPARKLE_ACCEL; break;
    case 3: pattern_ = OFF; break;
  }
  beginPattern();
}

///////////////////////////////////////////////
// Set LEDs

uint32_t color(byte red, byte green, byte blue) {
  return strip_.Color(red, green, blue);
}

#define LED_ROWS 3
#define LED_COLUMNS 10
byte LED_INDEX[LED_ROWS][LED_COLUMNS] = {
  { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9},
  {19, 18, 17, 16, 15, 14, 13, 12, 11, 10},
  {20, 21, 22, 23, 24, 25, 26, 27, 28, 29}
};


void logSetLed(int col, int row, uint32_t color) {
  Serial.print("Set LED ");
  Serial.print(col);
  Serial.print(" ");
  Serial.print(row);
  Serial.print(" pin ");
  Serial.print(LED_INDEX[row][col]);
  Serial.print(" ");
  Serial.println(color);
}

void setLed(int col, int row, uint32_t color) {
  // logSetLed(col, row, color);
  strip_.setPixelColor(LED_INDEX[row][col], color);
}

void setLedRow(int row, uint32_t color) {
  for (int i = 0; i < LED_COLUMNS; ++i) {
    setLed(i, row, color);
  }
}

void setLedColumn(int row, uint32_t color) {
  for (int i = 0; i < LED_ROWS; ++i) {
    setLed(i, row, color);
  }
}

void setLedAll(uint32_t color) {
  for (int i = 0; i < LED_ROWS; ++i) {
    setLedRow(i, color);
  }
}

///////////////////////////////////////////////
// Pattern processing.


void beginPattern() {
  Serial.print("Begin pattern");
  Serial.println(pattern_);
  switch(pattern_) {
    case SPARKLE_DIM: brightness_ = 2; beginSparklePattern(); break;
    case SPARKLE_BRIGHT: brightness_ = 10; beginSparklePattern(); break;
    case SPARKLE_ACCEL: brightness_ = 2; beginSparklePattern(); break;
    case OFF: beginOffPattern(); break;
  }
  strip_.show();
}

void updatePattern(long led_step) {
  //Serial.print("Update pattern: ");
  //Serial.print(pattern_);
  //Serial.print(" ");
  //Serial.println(led_step);
  switch (pattern_) {
    case SPARKLE_DIM:
    case SPARKLE_BRIGHT:
    case SPARKLE_ACCEL:
      updateSparklePattern(led_step); 
      break;
    case OFF: 
      return;
  }
  strip_.show();
}


#define LIT 10
uint8_t maxSteps = 100;
// Each item increases to a target color with a rate.
uint32_t targetColors[LIT];
uint32_t currentColors[LIT];
uint32_t colorStep[LIT];
uint16_t ledIndex[LIT];
int8_t ledUp[LIT];  // Positive for up, negative for down, 0 for unset.
uint8_t rBias, bBias, gBias;

long sparkle_prior_steps_ = 0;
long sparkle_cycle_ = 0;
// IMPLEMENT
void beginSparklePattern() {
  setLedAll(0);
  randomSeed(42);
  sparkle_prior_steps_ = 0;
  sparkle_cycle_ = 0;
  
  // Not sure if this is required, but makew sure the indexes are empty.
  for (uint16_t i = 0; i < LIT; ++i) {
    ledUp[i] = 0;
  }
  setLedAll(color(0, 0, 0));
}

uint32_t adjustSingleColor(uint32_t color, uint8_t shift) {
  uint32_t single_color = (color >> shift) & 0xFF;
  single_color = min(255, single_color * brightness_ / 10);
  return (color & ~(0xFF << shift)) | (single_color << shift);
}

uint32_t brightnessAdjustedColor(uint32_t color) {

  uint32_t adjusted = color;
  adjusted = adjustSingleColor(adjusted, 0);
  adjusted = adjustSingleColor(adjusted, 8);
  adjusted = adjustSingleColor(adjusted, 16);
  adjusted = adjustSingleColor(adjusted, 24);
  
  Serial.print("Adjusting color: ");
  Serial.print(color);
  Serial.print(" => ");
  Serial.println(adjusted);
}


void updateSparklePattern(long led_step) {
  sparkle_prior_steps_ += led_step;
  if (sparkle_prior_steps_ > 33) {
    Serial.println("Updating LEDs");
    sparkle_prior_steps_ = 0;
    ++sparkle_cycle_;

    if (sparkle_cycle_ % 300 == 0) {
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
        targetColors[i] = strip_.Color(r, g, b);
        colorStep[i] = strip_.Color(max(1, r / steps), max(1, g / steps), max(1, b / steps));
        currentColors[i] = strip_.Color(0, 0, 0);
        ledIndex[i] = random(strip_.numPixels());
      } 
      
      if (ledUp[i] > 0) {
        currentColors[i] += colorStep[i];
      } else {
        currentColors[i] -= colorStep[i];
      }
      
      strip_.setPixelColor(ledIndex[i], brightnessAdjustedColor(currentColors[i]));
      
      if (currentColors[i] > targetColors[i]) {
        ledUp[i] = -1;
      }
      
      if (currentColors[i] < colorStep[i]) {
        // Reset the index to restart the sequence.
        strip_.setPixelColor(ledIndex[i], 0);
        ledUp[i] = 0;
        ledIndex[i] = 0;
      }
      
    }
    strip_.show();
  }
}

void beginOffPattern() {
  setLedAll(color(0, 0, 0));
}
void updateOffPattern(long led_step) {
}



///////////////////////////////////////////////
/// Running code.
char last_key_ = 0;  // Need to track the last key to ensure
// it goes low before counting it as an action.
void updateByKeypad() {
  char pressed_key = keypad_.getKey();
  if (pressed_key && pressed_key != last_key_) {
    switch (pressed_key) {
      case '1':
        handleButton(0);
        break;
      case '2':
        handleButton(1);
        break;
      case '3':
        handleButton(2);
        break;
      case '4':
        handleButton(3);
        break;
    }
    Serial.print("Key Pressed: ");
    Serial.println(pressed_key);
  }
}

void printAccelerometerState() {
  Serial.print("Accel read: ");
  Serial.print(linear_acceleration_magnitude_);
  Serial.print(" x: ");
  Serial.print(orientation_.x);
  Serial.print(" y: ");
  Serial.print(orientation_.y);
  Serial.print(" z: ");
  Serial.println(orientation_.z);
}

long last_accel_read_ = 0;
void updateByAccelerometer() {
  if (time_ - last_accel_read_ >= BNO055_SAMPLERATE_DELAY_MS) {
    last_accel_read_ = time_;

    sensors_event_t event;
    bno.getEvent(&event);

    imu::Vector<3> lineacc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    linear_acceleration_magnitude_ = lineacc.magnitude();
    orientation_ = event.orientation;
    // nvbm,.printAccelerometerState();
    if (pattern_ == SPARKLE_ACCEL) {
      brightness_ = max(10, (linear_acceleration_magnitude_ / 10));
    }
  }
}

long last_led_update_ = millis();
void updateLEDs() {
  long led_step = time_ - last_led_update_;
  last_led_update_ = time_;

  updatePattern(led_step);
}

void loop() {
  // put your main code here, to run repeatedly:
  time_ = millis();
  //Serial.print("Loop: ");
  //Serial.println(time_);

  updateByKeypad();
  
  //Serial.print("keypad: ");
  updateByAccelerometer();
  
  //Serial.print("accel: ");
  updateLEDs();
  
  //Serial.print("led: ");
}



