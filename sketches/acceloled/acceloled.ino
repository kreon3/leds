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

// Pattern currently being displayed on the LEDs.
enum Pattern {
  PATTERN_SPARKLE = 0,
  PATTERN_ROW_MOVE = 1,
  PATTERN_COLOR_ALL = 2,
  OFF_PATTERN = 3,

  DENSITY_SPEED_PATTERN = 4,
  BRIGHTNESS_PATTERN = 5,
};
enum Pattern pattern_ = PATTERN_COLOR_ALL;

// Current state of the program.
enum State {
  RUN_PATTERN = 0,
  MODE_SELECT = 1,
  OFF_MODE = 2,
  PATTERN_SELECT = 3,
  PROGRAM_SELECT = 4,
  COLOR_SELECT = 5,
  SPEED_SELECT = 6,
  DENSITY_SELECT = 7,
  BRIGHTNESS_SELECT = 8
};
enum State state_ = RUN_PATTERN;

enum Mode {
  MOVEMENT_MODE = 0,
  TIME_MODE = 1
};
enum Mode mode_ = TIME_MODE;

// Pattern control variables go from 0-10.
uint8_t speed_ = 5;  // Speed that the pattern moves.
uint8_t density_ = 5;  // General number of LEDs to display.
uint8_t brightness_ = 5;  // Brightness of the strips.

// TODO: Sensitivity?

// Aspects influence the color displayed if the pattern utilizes it.
uint8_t red_aspect_ = 5;
uint8_t green_aspect_ = 5;
uint8_t blue_aspect_ = 5;

// Accelerometer readings.a
float linear_acceleration_magnitude_ = 0;
sensors_vec_t orientation_;  // key items are floats for roll, pitch, and heading.

void initState() {
  state_ = RUN_PATTERN;
  pattern_ = PATTERN_COLOR_ALL;
  mode_ = TIME_MODE;
  speed_ = 5;
  density_ = 5;
  brightness_ = 5;
  red_aspect_ = 5;
  green_aspect_ = 5;
  blue_aspect_ = 5;
  linear_acceleration_magnitude_ = 0;
}

///////////////////////////////////////////////
/// State modifiers.

void increaseSpeed() {
  if (speed_ < 10) {
    speed_++;
  }
}
void decreaseSpeed() {
  if (density_ > 0) {
    density_--;
  }
}
void increaseDensity() {
  if (density_ < 10) {
    density_++;
  }
}
void decreaseDensity() {
  if (density_ > 0) {
    density_--;
  }
}
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


/* Button inputs:
 *    1. Enter mode select
 *    2. Enter pattern select
 *    3. Program
 *    4. Cancel/Done
 *
 *  Mode Select
 *    1. Enter off mode
 *    2. Movement Mode - Patterns based on movement
 *    3. Time mode - Patterns based on time.
 *    4. Cancel/Done
 *
 *  Off Mode
 *    1-4: Enter run pattern
 *
 *  Pattern Select
 *    1. Pattern A
 *    2. Pattern B
 *    3. Pattern C
 *    4. Cancel/Done
 *    * Advanced uses 1-3 to base-3 select a pattern.
 *
 *  Program
 *    1. Enter Color Select
 *    2. Enter Density/Speed Select
 *    3. Enter Brightness Select
 *    4. Cancel/Done
 *
 *  Color Select - (Temp Pattern) All LEDs color.
 *    1. Red increase to 10 then reset.
 *    2. Green increase to 10 then reset.
 *    3. Blue increase to 10 then reset.
 *    4. Done
 *
 *  Density: (Temp Pattern) Moving bar on speed with count by density.
 *    1. To Speed Select.
 *    2. Decrease Density
 *    3. Increase Density
 *    4. Done
 *
 *  Speed Select:
 *    1. To Density Select
 *    2. Decrease Speed
 *    3. Increase Speed
 *    4. Done
 *
 *  Brightness Select:
 *    1. -
 *    2. Decrease Brightness
 *    3. Increase Brightness
 *    4. Done
 */


// Previous pattern used to track temporary pattern state to revert back to the prior.
enum Pattern previous_pattern_;


void handleButton(int button_index) {
  Serial.print("Handle Button ");
  Serial.println(button_index);
  switch (state_) {
    case RUN_PATTERN:
      switch (button_index) {
        case 0:
          state_ = MODE_SELECT;
          return;
        case 1:
          state_ = PATTERN_SELECT;
          return;
        case 2:
          state_ = PROGRAM_SELECT;
          return;
        case 3: returnToDisplay();
          return;
        default:
          return;
      }
    case MODE_SELECT:
      switch (button_index) {
        case 0:
          state_ = OFF_MODE;
          previous_pattern_ = pattern_;
          pattern_ = OFF_PATTERN;
          returnToDisplay();
          return;
        case 1:
          mode_ = MOVEMENT_MODE;
          returnToDisplay();
          return;
        case 2:
          mode_ = TIME_MODE;
          returnToDisplay();
          return;
        case 3: returnToDisplay();
          return;
        default:
          return;
      }
    case OFF_MODE:
      switch (button_index) {
        case 0:
        case 1:
        case 2:
        case 3:
          pattern_ = previous_pattern_;
          returnToDisplay();
          return;
        default:
          return;
      }
    case PATTERN_SELECT:
      switch (button_index) {
        case 0:
          pattern_ = PATTERN_SPARKLE;
          returnToDisplay();
          return;
        case 1:
          pattern_ = PATTERN_ROW_MOVE;
          returnToDisplay();
          return;
        case 2:
          pattern_ = PATTERN_COLOR_ALL;
          returnToDisplay();
          return;
        case 3: returnToDisplay();
          return;
        default:
          return;
      }
    case PROGRAM_SELECT:
      switch (button_index) {
        case 0:
          previous_pattern_ = pattern_;
          state_ = COLOR_SELECT;
          return;
        case 1:
          previous_pattern_ = pattern_;
          state_ = DENSITY_SELECT;
          pattern_ = DENSITY_SPEED_PATTERN;
          beginPattern();
          return;
        case 2:
          previous_pattern_ = pattern_;
          state_ = BRIGHTNESS_SELECT;
          pattern_ = BRIGHTNESS_PATTERN;
          beginPattern();
          return;
        case 3: returnToDisplay();
          return;
        default:
          return;
      }
    case COLOR_SELECT:
      // Disabled/Unimplemented.
      returnToDisplay();
      switch (button_index) {
        case 0:
          return;
        case 1:
          return;
        case 2:
          return;
        case 3: returnToDisplay();
          return;
        default:
          return;
      }
    case SPEED_SELECT:
      switch (button_index) {
        case 0:
          state_ = DENSITY_SELECT;
          return;
        case 1:
          decreaseSpeed();
          return;
        case 2:
          increaseSpeed();
          return;
        case 3:
          pattern_ = previous_pattern_;
          returnToDisplay();
          return;
        default:
          return;
      }
    case DENSITY_SELECT:
      switch (button_index) {
        case 0:
          state_ = SPEED_SELECT;
          return;
        case 1:
          decreaseDensity();
          return;
        case 2:
          increaseDensity();
          return;
        case 3:
          pattern_ = previous_pattern_;
          returnToDisplay();
          return;
        default:
          return;
      }
    case BRIGHTNESS_SELECT:
      switch (button_index) {
        case 0:
          // No-op.
          return;
        case 1:
          decreaseBrightness();
          return;
        case 2:
          increaseBrightness();
          return;
        case 3:
          pattern_ = previous_pattern_;
          returnToDisplay();
          return;
        default:
          return;
      }
  }
}

void returnToDisplay() {
  state_ = RUN_PATTERN;
  beginPattern();
}


///////////////////////////////////////////////
// Set LEDs

uint32_t color(byte red, byte green, byte blue) {
  return strip_.Color(red, green, blue);
}

// bias_ratio is a value between 0 and 1.
uint32_t biasedColor(byte red, byte green, byte blue, float bias_ratio) {
  float safe_bias_ratio = max(min(bias_ratio, 1), 0);
  float red_bias = float(red_aspect_) / 10 * bias_ratio;
  float green_bias = float(green_aspect_) / 10 * bias_ratio;
  float blue_bias = float(blue_aspect_) / 10 * bias_ratio;
  return color(red * red_bias, green * green_bias, blue * blue_bias);
}

uint32_t brightnessBiasedColor(byte red, byte green, byte blue, float bias_ratio) {
  float safe_bias_ratio = max(min(bias_ratio, 1), 0);
  float red_bias = float(red_aspect_) * float(brightness_) / 100 * bias_ratio;
  float green_bias = float(red_aspect_) * float(brightness_) / 100 * bias_ratio;
  float blue_bias = float(red_aspect_) * float(brightness_) / 100 * bias_ratio;
  return color(red * red_bias, green * green_bias, blue * blue_bias);
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
  switch (pattern_) {
    case PATTERN_SPARKLE:       beginSparklePattern(); break;
    case PATTERN_ROW_MOVE:      beginRowMovePattern(); break;
    case PATTERN_COLOR_ALL:     beginColorAllPattern(); break;
    case OFF_PATTERN:           beginOffPattern(); break;
    case DENSITY_SPEED_PATTERN: beginDensitySpeedPattern(); break;
    case BRIGHTNESS_PATTERN:    beginBrightnessPattern(); break;
  }
  strip_.show();
}

void updatePattern(long led_step) {
  // Switch step to be approximately equal to 1ms.
  long speed_step = led_step * 5 / speed_;
  
  //Serial.print("Update pattern: ");
  //Serial.print(pattern_);
  //Serial.print(" ");
  //Serial.println(led_step);
  switch (pattern_) {
    case PATTERN_SPARKLE:       updateSparklePattern(speed_step); break;
    case PATTERN_ROW_MOVE:      updateRowMovePattern(speed_step); break;
    case PATTERN_COLOR_ALL:     updateColorAllPattern(speed_step); break;
    case OFF_PATTERN:           updateOffPattern(speed_step); break;
    case DENSITY_SPEED_PATTERN: updateDensitySpeedPattern(speed_step); break;
    case BRIGHTNESS_PATTERN:    updateBrightnessPattern(speed_step); break;
  }
  strip_.show();
}
// IMPLEMENT
void beginSparklePattern() {
  
}
void updateSparklePattern(long led_step) {
}

// IMPLEMENT
void beginRowMovePattern() {
}
void updateRowMovePattern(long led_step) {
}

void beginColorAllPattern() {
  updateColorAllPattern(0);
}
void updateColorAllPattern(long led_step) {
  setLedAll(brightnessBiasedColor(255, 255, 255, 1.));
}


void beginOffPattern() {
  setLedAll(color(0, 0, 0));
}
void updateOffPattern(long led_step) {
}

// IMPLEMENT
int density_pos_;
long density_last_update_;
void beginDensitySpeedPattern() {
  density_pos_ = 0;
  density_last_update_ = 0;
}
void updateDensitySpeedPattern(long led_step) {
  long cur_update = led_step + density_last_update_;
  if (cur_update - density_last_update_ > 250) {
    density_last_update_ = cur_update;
    if (density_pos_ >= 10) {
      density_pos_ = 0;
    } else {
      ++density_pos_;
    }

    for (uint8_t i = 0; i < density_; ++i) {
      uint8_t cur_col = (density_pos_ + 1) % LED_COLUMNS;
      setLedColumn(biasedColor(255, 255, 255, 1.), cur_col);
    }
  }
}


void beginBrightnessPattern() {
}
void updateBrightnessPattern(long led_step) {
  setLedAll(biasedColor(255, 255, 255, 1.));
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
    if (mode_ == MOVEMENT_MODE) {
      // Acceleration appears to max around 40 off of movement
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



